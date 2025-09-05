import * as THREE from 'three';

// Helper functions
function createTextSprite(text: string, width = 64, height = 32): THREE.Sprite {
    const canvas = document.createElement('canvas');
    const context = canvas.getContext('2d')!;
    canvas.width = width;
    canvas.height = height;
    context.font = '16px Arial';
    context.fillStyle = 'white';
    context.textAlign = 'center';
    context.fillText(text, width / 2, height / 2 + 4);

    const texture = new THREE.CanvasTexture(canvas);
    const material = new THREE.SpriteMaterial({ map: texture });
    return new THREE.Sprite(material);
}

function createVerticalTextSprite(text: string): THREE.Sprite {
    const canvas = document.createElement('canvas');
    const context = canvas.getContext('2d')!;
    canvas.width = 40;
    canvas.height = 80;
    context.font = '16px Arial';
    context.fillStyle = 'white';
    context.textAlign = 'center';
    context.textBaseline = 'middle';
    context.translate(20, 40);
    context.rotate(-Math.PI / 2);
    context.fillText(text, 0, 0);

    const texture = new THREE.CanvasTexture(canvas);
    const material = new THREE.SpriteMaterial({ map: texture });
    const sprite = new THREE.Sprite(material);
    sprite.scale.set(10, 20, 1);
    return sprite;
}

// Dynamic data structure for any number of data fields
const dataFields = new Map<string, {
    positions: Float32Array;
    geometry: THREE.BufferGeometry;
    line: THREE.Line;
    color: number;
}>();

const fieldColors = [0xffff00, 0x00ff00, 0x00ffff, 0xff00ff, 0xff8000, 0x8000ff]; // Colors for data fields
let colorIndex = 0;

function createDataField(fieldName: string): void {
    const positions = new Float32Array(MAX_POINTS * 3);
    const geometry = new THREE.BufferGeometry();
    geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));

    const color = fieldColors[colorIndex % fieldColors.length];
    const material = new THREE.LineBasicMaterial({ color });
    const line = new THREE.Line(geometry, material);

    scene.add(line);

    dataFields.set(fieldName, {
        positions,
        geometry,
        line,
        color
    });

    colorIndex++;
}

function shiftDataArrays() {
    for (let i = 0; i < MAX_POINTS - 1; i++) {
        dataFields.forEach(field => {
            field.positions[i * 3] = field.positions[(i + 1) * 3];
            field.positions[i * 3 + 1] = field.positions[(i + 1) * 3 + 1];
        });
        timestamps[i] = timestamps[i + 1];
    }
    pointCount--;
    oldestTimestamp = timestamps[0];
}

function recalculatePositions() {
    for (let i = 0; i < pointCount; i++) {
        const x = (timestamps[i] - oldestTimestamp) / 100;
        dataFields.forEach(field => {
            field.positions[i * 3] = x;
        });
    }
}

function updateMinMaxValues() {
    minValue = Infinity;
    maxValue = -Infinity;
    for (let i = 0; i < pointCount; i++) {
        dataFields.forEach(field => {
            const value = field.positions[i * 3 + 1];
            minValue = Math.min(minValue, value);
            maxValue = Math.max(maxValue, value);
        });
    }
}

function updateTimeLabelsPositions() {
    timeLabels.forEach(label => {
        const originalTime = parseFloat(label.userData?.originalTime || '0');
        const newX = (originalTime - oldestTimestamp) / 100;
        label.position.x = newX;
        label.visible = newX >= 0;
    });
}

function updateDataGeometry() {
    dataFields.forEach(field => {
        field.geometry.setDrawRange(0, pointCount);
        field.geometry.attributes.position.needsUpdate = true;
        field.geometry.computeBoundingSphere();
    });
}

function updateLineScaleAndPosition() {
    const yRange = maxValue - minValue;
    const yCenter = (maxValue + minValue) / 2;
    const scaleY = yRange > 0 ? 100 / yRange : 1;
    const posY = -yCenter * scaleY;

    dataFields.forEach(field => {
        field.line.scale.y = scaleY;
        field.line.position.y = posY;
    });

    return { scaleY, posY };
}

function addDataPoint(time: number, values: Record<string, number>) {
    const x = (time - oldestTimestamp) / 100;
    const index = pointCount * 3;

    // Ensure all fields exist
    Object.keys(values).forEach(fieldName => {
        if (!dataFields.has(fieldName)) {
            createDataField(fieldName);
        }
    });

    // Add data points
    dataFields.forEach((field, fieldName) => {
        field.positions[index] = x;
        field.positions[index + 1] = values[fieldName] ?? 0;
    });

    timestamps[pointCount] = time;
    pointCount++;
}

function createLegendLabels() {
    let yPos = 40;
    dataFields.forEach((field, fieldName) => {
        const colorHex = `#${field.color.toString(16).padStart(6, '0')}`;
        const legend = createLegendLabel(fieldName, colorHex, yPos);
        scene.add(legend);
        yPos -= 15; // Stack legends vertically
    });
}

function disposeSprite(sprite: THREE.Sprite | null) {
    if (sprite) {
        scene.remove(sprite);
        sprite.material.map?.dispose();
        sprite.material.dispose();
    }
}

// Renderer setup
const canvas = document.getElementById('glcanvas') as HTMLCanvasElement;
const renderer = new THREE.WebGLRenderer({ canvas });
renderer.setSize(window.innerWidth, window.innerHeight);
renderer.setClearColor(0x000000, 1);

// Scene & camera
const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
const gridSizeX = 200;
camera.position.set(gridSizeX / 2, 0, 100); // Center on middle of grid

// Mouse controls
let isMouseDown = false;
let mouseX = 0;
let mouseY = 0;

canvas.addEventListener('mousedown', (event) => {
    isMouseDown = true;
    mouseX = event.clientX;
    mouseY = event.clientY;
});

canvas.addEventListener('mousemove', (event) => {
    if (isMouseDown) {
        const deltaX = event.clientX - mouseX;
        const deltaY = event.clientY - mouseY;

        camera.position.x -= deltaX * 0.1;
        camera.position.y += deltaY * 0.1;

        mouseX = event.clientX;
        mouseY = event.clientY;
    }
});

canvas.addEventListener('mouseup', () => {
    isMouseDown = false;
});

canvas.addEventListener('wheel', (event) => {
    event.preventDefault();
    camera.position.z += event.deltaY * 0.1;
    camera.position.z = Math.max(1, camera.position.z);
});

// Axes & grid
scene.add(new THREE.AxesHelper(100));

// Declare variables before they are used in updateGrid
let pointCount = 0;
let minValue = Infinity;  // Shared min for all three
let maxValue = -Infinity; // Shared max for all three
let firstTimestamp = -1;
let lastLabelTimestamp = -1;
let t0 = -1;
let oldestTimestamp = -1; // Track the oldest timestamp in the buffer

// Create custom grid for positive x only
const gridSizeY = 100; // Half height
const divisionsX = 20;
const divisionsY = 10; // Even fewer divisions for y-axis
const gridGeometry = new THREE.BufferGeometry();
const maxGridVertices = (divisionsX + 1) * 4 + (divisionsY + 1) * 4; // Max possible vertices
let gridVertices = new Float32Array(maxGridVertices * 3);
let gridVertexCount = 0;

function updateGrid() {
    gridVertexCount = 0;

    // Calculate x offset based on oldest timestamp
    const xOffset = oldestTimestamp !== -1 ? (oldestTimestamp - t0) / 100 : 0;
    const gridStepX = gridSizeX / divisionsX; // 10 units per division

    // Calculate which grid lines should be visible
    // Start from the first grid line that would be visible
    const firstVisibleLine = Math.floor(xOffset / gridStepX);
    const lastVisibleLine = Math.ceil((xOffset + gridSizeX) / gridStepX);

    // Vertical lines (parallel to y-axis) - only show lines within visible range
    for (let i = firstVisibleLine; i <= lastVisibleLine; i++) {
        const x = i * gridStepX - xOffset;
        if (x >= 0 && x <= gridSizeX) { // Only show lines within the exact visible range
            gridVertices[gridVertexCount * 3] = x;
            gridVertices[gridVertexCount * 3 + 1] = -gridSizeY / 2;
            gridVertices[gridVertexCount * 3 + 2] = 0;
            gridVertexCount++;

            gridVertices[gridVertexCount * 3] = x;
            gridVertices[gridVertexCount * 3 + 1] = gridSizeY / 2;
            gridVertices[gridVertexCount * 3 + 2] = 0;
            gridVertexCount++;
        }
    }

    // Horizontal lines (parallel to x-axis)
    for (let i = 0; i <= divisionsY; i++) {
        const y = (i / divisionsY) * gridSizeY - gridSizeY / 2;
        gridVertices[gridVertexCount * 3] = 0;
        gridVertices[gridVertexCount * 3 + 1] = y;
        gridVertices[gridVertexCount * 3 + 2] = 0;
        gridVertexCount++;

        gridVertices[gridVertexCount * 3] = gridSizeX;
        gridVertices[gridVertexCount * 3 + 1] = y;
        gridVertices[gridVertexCount * 3 + 2] = 0;
        gridVertexCount++;
    }

    gridGeometry.setAttribute('position', new THREE.BufferAttribute(gridVertices, 3));
    gridGeometry.setDrawRange(0, gridVertexCount);
    gridGeometry.attributes.position.needsUpdate = true;
}

// Initial grid setup
updateGrid();
const gridMaterial = new THREE.LineBasicMaterial({ color: 0x444444 });
const grid = new THREE.LineSegments(gridGeometry, gridMaterial);
scene.add(grid);

// Time axis labels
const timeLabels: THREE.Sprite[] = [];

// Y-axis labels
let minYawLabel: THREE.Sprite | null = null;
let maxYawLabel: THREE.Sprite | null = null;
let zeroYawLabel: THREE.Sprite | null = null;

// Axis title labels
let timeAxisLabel: THREE.Sprite | null = null;
let valueAxisLabel: THREE.Sprite | null = null;

// Legend labels
// let yawLegend: THREE.Sprite | null = null;
// let pitchLegend: THREE.Sprite | null = null;
// let rollLegend: THREE.Sprite | null = null;

// Line for dynamic data fields vs Time
const MAX_POINTS = 1000;
const timestamps = new Float32Array(MAX_POINTS); // Store timestamps for each point

// Helper function to create legend label
function createLegendLabel(text: string, color: string, yPos: number): THREE.Sprite {
    const canvas = document.createElement('canvas');
    const context = canvas.getContext('2d')!;
    canvas.width = 120;
    canvas.height = 32;

    // Draw colored line
    context.strokeStyle = color;
    context.lineWidth = 3;
    context.beginPath();
    context.moveTo(5, 16);
    context.lineTo(25, 16);
    context.stroke();

    // Draw text
    context.font = '16px Arial';
    context.fillStyle = 'white';
    context.textAlign = 'left';
    context.fillText(text, 30, 20);

    const texture = new THREE.CanvasTexture(canvas);
    const material = new THREE.SpriteMaterial({ map: texture });
    const sprite = new THREE.Sprite(material);
    const gridStepX = gridSizeX / 20; // One grid step (10 units)
    sprite.position.set(gridSizeX + gridStepX * 2, yPos, 0); // 2 grid steps to the right
    sprite.scale.set(25, 8, 1);
    return sprite;
}

// Create axis title labels (only once)
let axisLabelsCreated = false;
if (!axisLabelsCreated) {
    // Create axis title labels
    const timeLabel = createTextSprite('Time (s)', 80, 32);
    timeLabel.position.set(gridSizeX / 2, -70, 0);
    timeLabel.scale.set(20, 8, 1);
    scene.add(timeLabel);

    const valueLabel = createVerticalTextSprite('Value');
    valueLabel.position.set(-20, 0, 0);
    scene.add(valueLabel);

    axisLabelsCreated = true;
}

// WebSocket connection
const socket = new WebSocket('ws://localhost:3000');
socket.onopen = () => console.log('WebSocket connected');
socket.onmessage = ev => {
    const { time, values } = JSON.parse(ev.data);

    if (t0 === -1) {
        t0 = time;
        firstTimestamp = time;
        lastLabelTimestamp = time;
        oldestTimestamp = time;

        const initialSprite = createTextSprite('0');
        initialSprite.position.set(0, -60, 0);
        initialSprite.scale.set(16, 8, 1);
        initialSprite.userData = { originalTime: time };
        scene.add(initialSprite);
        timeLabels.push(initialSprite);
    }

    // When buffer is full, shift points left
    if (pointCount === MAX_POINTS) {
        shiftDataArrays();
        recalculatePositions();
        updateGrid();
        updateTimeLabelsPositions();
        updateMinMaxValues();
    }

    // Add new data point
    addDataPoint(time, values);

    // Remove data points older than 20 seconds
    const timeLimit = 20000;
    while (pointCount > 1 && (time - timestamps[0]) > timeLimit) {
        shiftDataArrays();
        recalculatePositions();
        updateGrid();
        updateTimeLabelsPositions();
        updateMinMaxValues();
    }

    // Update min/max with new values - fix TypeScript error
    Object.values(values).forEach((value: unknown) => {
        const numValue = Number(value);
        if (!isNaN(numValue)) {
            minValue = Math.min(minValue, numValue);
            maxValue = Math.max(maxValue, numValue);
        }
    });

    // Create legends if this is the first data point
    if (pointCount === 1) {
        createLegendLabels();
    }

    // Add time labels
    if (time - lastLabelTimestamp >= 1000) {
        const elapsedTime = time - firstTimestamp;
        const x = (time - oldestTimestamp) / 100;
        let reuseLabel = timeLabels.find(label => !label.visible);

        if (reuseLabel) {
            const newSprite = createTextSprite(`${Math.round(elapsedTime / 1000)}`);
            (reuseLabel.material as THREE.SpriteMaterial).map = newSprite.material.map;
            reuseLabel.position.set(x, -60, 0);
            reuseLabel.userData = { originalTime: time };
            reuseLabel.visible = true;
        } else {
            const sprite = createTextSprite(`${Math.round(elapsedTime / 1000)}`);
            sprite.position.set(x, -60, 0);
            sprite.scale.set(16, 8, 1);
            sprite.userData = { originalTime: time };
            scene.add(sprite);
            timeLabels.push(sprite);
        }
        lastLabelTimestamp = time;
    }

    updateDataGeometry();
    const { scaleY, posY } = updateLineScaleAndPosition();

    // Helper function to create y-axis label
    function createYAxisLabel(value: number, yPos: number, x: number = -10): THREE.Sprite {
        const sprite = createTextSprite(value.toFixed(3));
        sprite.position.set(x, yPos, 0);
        sprite.scale.set(16, 8, 1);
        return sprite;
    }

    // Update y-axis labels - rename variables to be generic
    disposeSprite(minYawLabel);
    minYawLabel = createYAxisLabel(minValue, posY + minValue * scaleY);
    scene.add(minYawLabel);

    disposeSprite(maxYawLabel);
    maxYawLabel = createYAxisLabel(maxValue, posY + maxValue * scaleY);
    scene.add(maxYawLabel);

    // Zero label
    if (minValue <= 0 && maxValue >= 0) {
        disposeSprite(zeroYawLabel);
        zeroYawLabel = createYAxisLabel(0, posY);
        scene.add(zeroYawLabel);
    } else {
        disposeSprite(zeroYawLabel);
        zeroYawLabel = null;
    }
};

// Resize handler
window.addEventListener('resize', () => {
    renderer.setSize(window.innerWidth, window.innerHeight);
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
});

// Animation
function animate() {
    requestAnimationFrame(animate);
    renderer.render(scene, camera);
}
animate();