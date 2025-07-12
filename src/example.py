import sys
import re
import subprocess
import threading
import queue
import pyqtgraph as pg
from pyqtgraph.Qt import QtWidgets, QtCore

# Parse script argument or use default
DEFAULT_SCRIPT = 'src/example.ts'
script_path = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_SCRIPT

# Configuration
TIMEOUT_LIMIT_START = 250
TIMEOUT_LIMIT_RUN = 25
UPDATE_INTERVAL_MS = 40  # 25 FPS

# Data queue and process handle
data_q = queue.Queue()
proc = None

t0 = None
N_samples = 0

# Regex patterns
float_re = re.compile(r'[-+]?[0-9]*\.?[0-9]+')

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        # No-data counter and timeout
        self.no_data_rounds = 0
        self.timeout_limit = TIMEOUT_LIMIT_START

        # Storage for curves and data buffers
        self.curves = {}
        self.buffers = {}

        # Plot setup
        self.plot_widget = pg.PlotWidget(title="Sensor")
        # Set axis labels
        self.plot_widget.setLabel('bottom', 'Time (ms)')
        self.plot_widget.setLabel('left', 'Value')
        self.legend = self.plot_widget.addLegend()
        self.legend.hide()

        # Initial state
        self.set_state("INIT")

        self.setCentralWidget(self.plot_widget)

        # Show and start subprocess
        self.show()
        QtCore.QTimer.singleShot(0, self.start_subprocess)

        # Timer for updates
        self.timer = QtCore.QTimer(self)
        self.timer.timeout.connect(self.update_plot)
        self.timer.start(UPDATE_INTERVAL_MS)

    def start_subprocess(self):
        global proc
        proc = subprocess.Popen(
            ['stdbuf', '-oL', 'ts-node', script_path],
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            text=True, bufsize=1
        )
        self.set_state('START')
        threading.Thread(target=self.reader, daemon=True).start()

    def set_state(self, state):
        self.setWindowTitle(f"{state} {script_path}")
        self.state = state

    def reader(self):
        global t0, N_samples
        sample = {}
        while True:
            line = proc.stdout.readline()
            if not line and proc.poll() is not None:
                break
            if line.startswith('{'):
                continue
            parts = re.split(r'(, ?| ?-- ?)', line.strip())
            for part in parts[1:]:
                subparts = re.split(r': ?', part)
                if subparts[0] == 'Delay':
                    continue
                key = 'Time' if subparts[0] == 'Time' else parts[0] + ', ' + subparts[0]
                float_match = float_re.search(subparts[-1])
                if float_match:
                    sample[key] = float(float_match.group())
            # Enqueue when at least one measurement exist
            if len(sample) > 1:
                if t0 is None:
                    t0 = sample.get('Time', 0)
                data_q.put((sample.get('Time', N_samples*UPDATE_INTERVAL_MS) - t0, {k: v for k, v in sample.items() if k != 'Time'}))
                sample = {}
                N_samples += 1

    def update_plot(self):
        got_data = False
        while True:
            try:
                t, values = data_q.get_nowait()
            except queue.Empty:
                break
            got_data = True
            if self.state != 'RUN':
                self.set_state("RUN")
                self.timeout_limit = TIMEOUT_LIMIT_RUN
                self.legend.show()
            # For each measurement key
            for key, val in values.items():
                # Initialize curve and buffer if first time
                if key not in self.curves:
                    curve = self.plot_widget.plot()
                    self.curves[key] = curve
                    self.buffers[key] = {'T': [], 'V': []}
                    self.legend.addItem(curve, key)
                    # Update colors
                    for i, c in enumerate(self.curves.values()):
                        c.setPen(pg.intColor(i, hues=len(self.curves)))
                # Append data
                buf = self.buffers[key]
                buf['T'].append(t)
                buf['V'].append(val)

        # Timeout logic
        if got_data:
            self.no_data_rounds = 0
        else:
            self.no_data_rounds += 1
            if self.state == 'RUN' and self.no_data_rounds >= self.timeout_limit:
                self.set_state('STOP')
                if proc and proc.poll() is None:
                    proc.terminate()

        # Redraw all curves
        if self.curves:
            margin = 0.05
            # Determine global time range over all keys
            all_T_min = min([t for buf in self.buffers.values() for t in buf['T']])
            all_T_max = max([t for buf in self.buffers.values() for t in buf['T']])
            all_T_diff = all_T_max - all_T_min
            if all_T_diff != 0:
                self.plot_widget.setXRange(all_T_min - all_T_diff*margin, all_T_max + all_T_diff*margin, padding=0)
            # Determine global value range
            all_V_min = min([v for buf in self.buffers.values() for v in buf['V']])
            all_V_max = max([v for buf in self.buffers.values() for v in buf['V']])
            all_V_diff = all_V_max - all_V_min
            if all_V_diff != 0:
                self.plot_widget.setYRange(all_V_min - all_V_diff*margin, all_V_max + all_V_diff*margin, padding=0)
            # Update curve data
            for key, curve in self.curves.items():
                buf = self.buffers[key]
                curve.setData(buf['T'], buf['V'])

    def closeEvent(self, event):
        if proc and proc.poll() is None:
            proc.terminate()
        event.accept()

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    win = MainWindow()
    sys.exit(app.exec())
