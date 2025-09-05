
import * as THREE from "three"

// Renderer setup
const canvas = document.getElementById('glcanvas') as HTMLCanvasElement
const renderer = new THREE.WebGLRenderer({ canvas })
renderer.setSize(window.innerWidth, window.innerHeight)
renderer.setClearColor(0x000000, 1)

// Scene & camera
const scene = new THREE.Scene()
const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000)
const gridSizeX = 200
camera.position.set(gridSizeX / 2, 0, 100) // Center on middle of grid

// Model
const geometry = new THREE.BoxGeometry()
const material = new THREE.MeshNormalMaterial({ wireframe: true })
const cube = new THREE.Mesh(geometry, material)
scene.add(cube)

// Mouse controls
let isMouseDown = false
let mouseX = 0
let mouseY = 0

// WebSocket connection
const socket = new WebSocket('ws://localhost:3000')
socket.onopen = () => console.log('WebSocket connected')
socket.onmessage = ev => {
    console.log(ev.data)
}

// Resize handler
window.addEventListener('resize', () => {
    renderer.setSize(window.innerWidth, window.innerHeight)
    camera.aspect = window.innerWidth / window.innerHeight
    camera.updateProjectionMatrix()
})

// Animation
function animate() {
    requestAnimationFrame(animate)
    renderer.render(scene, camera)
}
animate()