import express from 'express';
import path from 'path';
import { WebSocketServer } from 'ws';
import { startSensor, sensorStream } from './example_server_client';

// Serve static files from dist/
const distPath = path.join(__dirname);
const app = express();
app.use(express.static(distPath));
app.get('/', (_req, res) => {
    res.sendFile(path.join(distPath, 'client.html'));
});

const httpServer = app.listen(3000, () => {
    console.log('Listening on http://localhost:3000');
});

// WebSocket for sensor data
const wss = new WebSocketServer({ server: httpServer });
wss.on('connection', ws => {
    sensorStream.on('data', data => {
        ws.send(JSON.stringify(data));
    });
});

// Start polling sensor
startSensor();