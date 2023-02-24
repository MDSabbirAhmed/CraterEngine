// There are various techniques used for fluid simulation, 
// and one of them is the Smoothed Particle Hydrodynamics (SPH) method. 
//   The SPH method is a meshless Lagrangian method that uses particles to represent the fluid. 
//   In this approach, the fluid is represented as a set of particles 
//   that interact with each other according to certain physical laws.
// 
// To accelerate the simulation, we can use CUDA to parallelize the computations.
//   CUDA is a parallel computing platform and programming model developed by NVIDIA.
//   It allows us to perform computations on NVIDIA GPUs in parallel,
// which can significantly speed up the simulation.







#include <iostream>
#include <vector>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

const int N = 10000;             // Number of particles
const float dt = 0.001f;         // Time step
const float h = 0.02f;           // Smoothing length
const float rho0 = 1000.0f;      // Reference density
const float k = 1000.0f;         // Fluid constant
const float mu = 0.1f;           // Fluid constant
const float3 g = make_float3(0.0f, -9.81f, 0.0f); // Gravity

struct Particle {
    float3 pos;
    float3 vel;
    float3 acc;
    float rho;
    float pressure;
    float mass;

    Particle() : mass(0.02f) {}
};

__global__ void compute_density_and_pressure(Particle* particles, float* densities, float* pressures) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= N) return;

    float density = 0.0f;
    for (int j = 0; j < N; j++) {
        if (i == j) continue;

        float3 r = particles[j].pos - particles[i].pos;
        float r_len = length(r);
        density += particles[j].mass * (315.0f / (64.0f * M_PI * pow(h, 9))) * pow(h * h - r_len * r_len, 3);
    }

    densities[i] = density;
    pressures[i] = k * (densities[i] - rho0);
}

__global__ void compute_forces(Particle* particles, float* densities, float3* forces) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= N) return;

    float3 f_pressure = make_float3(0.0f, 0.0f, 0.0f);
    float3 f_viscosity = make_float3(0.0f, 0.0f, 0.0f);

    for (int j = 0; j < N; j++) {
        if (i == j) continue;

        float3 r = particles[j].pos - particles[i].pos;
        float r_len = length(r);
        if (r_len < h) {
            float3 u = (particles[j].vel - particles[i].vel) / (0.5f * (densities[i] + densities[j]));
            f_pressure += -r * particles[j].mass * (45.0f / (M_PI * pow(h, 6))) * (pressures[i] / pow(densities[i], 2) + pressures[j] / pow(densities[j], 2) - 2.0f * rho0 / (densities[i] + densities[j]));
            f_viscosity += mu * particles[j].mass * (45.0f / (M_PI * pow(h, 6))) * (u * r) * (particles[i].vel - particles[j].vel) / (r_len * r_len + 0.01f * h * h);
        }
    }

    float3 f_gravity = particles[i].mass * g;
    forces[i] = f_pressure + f_viscosity + f_gravity;
}

__global__ void update_particles(int N, float dt, float3* positions, float3* velocities, float3* forces, float3* colors) {
int i = blockIdx.x * blockDim.x + threadIdx.x;
if (i >= N) return;
  
  
  
  
  
  // Update velocity
velocities[i] += forces[i] / densities[i] * dt;

// Update position
positions[i] += velocities[i] * dt;

// Apply boundary conditions
if (positions[i].x < -box_size) {
    positions[i].x = -box_size + 0.1f;
    velocities[i].x *= -0.5f;
}
if (positions[i].x > box_size) {
    positions[i].x = box_size - 0.1f;
    velocities[i].x *= -0.5f;
}
if (positions[i].y < -box_size) {
    positions[i].y = -box_size + 0.1f;
    velocities[i].y *= -0.5f;
}
if (positions[i].y > box_size) {
    positions[i].y = box_size - 0.1f;
    velocities[i].y *= -0.5f;
}
if (positions[i].z < -box_size) {
    positions[i].z = -box_size + 0.1f;
    velocities[i].z *= -0.5f;
}
if (positions[i].z > box_size) {
    positions[i].z = box_size - 0.1f;
    velocities[i].z *= -0.5f;
}

// Update color based on velocity
float3 v = velocities[i];
float speed = length(v);
if (speed > max_speed) max_speed = speed;
colors[i] = make_float3(0.5f + v.x / (2.0f * max_speed), 0.5f + v.y / (2.0f * max_speed), 0.5f + v.z / (2.0f * max_speed));
}





int main() {
// Initialize particles
std::vector<Particle> particles(N);
for (int i = 0; i < N; i++) {
particles[i].pos = make_float3(drand48() * 2 * box_size - box_size, drand48() * 2 * box_size - box_size, drand48() * 2 * box_size - box_size);
particles[i].vel = make_float3(0, 0, 0);
particles[i].mass = 1.0f;
}
  
  
  
  // Copy particle data to device memory
float3* d_positions;
float3* d_velocities;
float* d_densities;
float* d_pressures;
float3* d_forces;
float3* d_colors;
cudaMalloc(&d_positions, N * sizeof(float3));
cudaMalloc(&d_velocities, N * sizeof(float3));
cudaMalloc(&d_densities, N * sizeof(float));
cudaMalloc(&d_pressures, N * sizeof(float));
cudaMalloc(&d_forces, N * sizeof(float3));
cudaMalloc(&d_colors, N * sizeof(float3));
cudaMemcpy(d_positions, &particles[0].pos, N * sizeof(float3), cudaMemcpyHostToDevice);
cudaMemcpy(d_velocities, &particles[0].vel, N * sizeof(float3), cudaMemcpyHostToDevice);

  
  // Initialize OpenGL and GLFW
GLFWwindow* window;
if (!glfwInit()) return -1;
glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
window = glfwCreateWindow(WIDTH, HEIGHT, "SPH Simulation", NULL, NULL);
if (!window) {
    glfwTerminate();
    return -1;
}
glfwMakeContextCurrent(window);

// Initialize OpenGL extensions with GLAD
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    return -1;
}

// Initialize shaders and buffers
Shader shader("shader.vert", "shader.frag");
GLuint vao, vbo_pos, vbo_col;
glGenVertexArrays(1, &vao);
glBindVertexArray(vao);
glGenBuffers(1, &vbo_pos);
glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
glBufferData(GL_ARRAY_BUFFER, N * sizeof(float3), NULL, GL_DYNAMIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float3), (void*)0);
glEnableVertexAttribArray(0);
glGenBuffers(1, &vbo_col);
glBindBuffer(GL_ARRAY_BUFFER, vbo_col);
glBufferData(GL_ARRAY_BUFFER, N * sizeof(float3), NULL, GL_DYNAMIC_DRAW);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float3), (void*)0);
glEnableVertexAttribArray(1);
glBindVertexArray(0);

// Simulation loop
float time = 0.0f;
while (!glfwWindowShouldClose(window)) {
    // Update particles on the device
    update_particles<<<blocks_per_grid, threads_per_block>>>(d_positions, d_velocities, d_densities, d_pressures, d_forces, N, h, rho0, mu, g, dt);
    cudaDeviceSynchronize();

    // Copy particle data from device memory to host memory
    cudaMemcpy(&particles[0].pos, d_positions, N * sizeof(float3), cudaMemcpyDeviceToHost);
    cudaMemcpy(&particles[0].vel, d_velocities, N * sizeof(float3), cudaMemcpyDeviceToHost);
    cudaMemcpy(&densities[0], d_densities, N * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(&pressures[0], d_pressures, N * sizeof(float), cudaMemcpyDeviceToHost);

    // Update particle colors based on density
    for (int i = 0; i < N; i++) {
        float3 col;
        if (densities[i] < rho0) {
            col = lerp(float3(0.0f, 1.0f, 0.0f), float3(1.0f, 0.0f, 0.0f), densities[i] / rho0);
        } else {
            col = lerp(float3(1.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 0.0f), (densities[i] - rho0) / rho0);
        }
        particles[i].col = col;
    }

    // Copy particle colors to device memory
    cudaMemcpy(d_colors, &particles[0].col, N * sizeof(float3), cudaMemcpyHostToDevice);

    // Update vertex buffer objects
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    glBufferSubData(GL_ARRAY_BUFFER, 0, N * sizeof(float3), &particles[0].pos);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_col);
    glBufferSubData(GL_ARRAY_Data(GL_ARRAY_BUFFER, 0, N * sizeof(float3), &particles[0].col);
                    
                    
                    // Draw particles
glClear(GL_COLOR_BUFFER_BIT);
glDrawArrays(GL_POINTS, 0, N);
glfwSwapBuffers(window);
                    
                    
                    }

int main() {
// Set up simulation parameters
const int N = 1000;
const float h = 0.02f;
const float rho0 = 1000.0f;
const float k = 1e3f;
const float mu = 0.1f;
const float dt = 0.01f;
const float3 g = make_float3(0.0f, -9.81f, 0.0f);


  
  
  
  // Set up initial particle positions and velocities
std::vector<Particle> particles(N);
for (int i = 0; i < N; i++) {
    particles[i].pos = make_float3(i % 50 * h - 0.5f, i / 50 * h - 5.0f, 0.0f);
    particles[i].vel = make_float3(0.0f, 0.0f, 0.0f);
    particles[i].mass = rho0 * h * h;
    particles[i].col = make_float3(0.0f, 0.0f, 1.0f);
}

// Set up GLFW window
GLFWwindow* window;
if (!glfwInit()) return -1;
window = glfwCreateWindow(640, 480, "Fluid Simulation", NULL, NULL);
if (!window) {
    glfwTerminate();
    return -1;
}
glfwMakeContextCurrent(window);

// Set up GLEW
GLenum err = glewInit();
if (err != GLEW_OK) {
    glfwTerminate();
    return -1;
}

// Set up vertex array object and vertex buffer objects
GLuint vao, vbo_pos, vbo_col;
glGenVertexArrays(1, &vao);
glBindVertexArray(vao);
glGenBuffers(1, &vbo_pos);
glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
glBufferData(GL_ARRAY_BUFFER, N * sizeof(float3), &particles[0].pos, GL_DYNAMIC_DRAW);
glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
glGenBuffers(1, &vbo_col);
glBindBuffer(GL_ARRAY_BUFFER, vbo_col);
glBufferData(GL_ARRAY_BUFFER, N * sizeof(float3), &particles[0].col, GL_DYNAMIC_DRAW);
glEnableVertexAttribArray(1);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
glBindVertexArray(0);

// Copy particle data to device memory
float3* d_positions;
float3* d_velocities;
float* d_densities;
float* d_pressures;
float3* d_forces;
float3* d_colors;
cudaMalloc(&d_positions, N * sizeof(float3));
cudaMalloc(&d_velocities, N * sizeof(float3));
cudaMalloc(&d_densities, N * sizeof(float));
cudaMalloc(&d_pressures, N * sizeof(float));
cudaMalloc(&d_forces, N * sizeof(float3));
cudaMalloc(&d_colors, N * sizeof(float3));
cudaMemcpy(d_positions, &particles[0].pos, N * sizeof(float3), cudaMemcpyHostToDevice);
cudaMemcpy(d_velocities, &particles[0].vel, N * sizeof(float3), cudaMemcpyHostToDevice);
cudaMemcpy(d_densities, densities, N * sizeof(float), cudaMemcpyHostToDevice);
cudaMemcpy(d_pressures, pressures, N * sizeof(float), cudaMemcpyHostToDevice);

// Start simulation loop
while (!glfwWindowShouldClose(window)) {
// Compute grid dimensions and block dimensions
dim3 gridDim((N + BLOCK_SIZE - 1) / BLOCK_SIZE);
dim3 blockDim(BLOCK_SIZE);
  
  
  // Compute density and pressure
compute_density_pressure<<<gridDim, blockDim>>>(d_positions, d_densities, d_pressures, N, h, rho0);
cudaDeviceSynchronize();

// Compute forces
compute_forces<<<gridDim, blockDim>>>(d_positions, d_velocities, d_densities, d_pressures, d_forces, N, h, rho0, mu, g);
cudaDeviceSynchronize();

// Update positions and velocities
update_particles<<<gridDim, blockDim>>>(d_positions, d_velocities, d_forces, dt, N);
cudaDeviceSynchronize();

// Copy particle data from device memory to host memory
cudaMemcpy(&particles[0].pos, d_positions, N * sizeof(float3), cudaMemcpyDeviceToHost);
cudaMemcpy(&particles[0].vel, d_velocities, N * sizeof(float3), cudaMemcpyDeviceToHost);

// Update particle colors
update_colors(particles, N, col_scale);

// Copy particle colors to device memory
cudaMemcpy(d_colors, &particles[0].col, N * sizeof(float3), cudaMemcpyHostToDevice);

// Update vertex buffer objects
glBindVertexArray(vao);
glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
glBufferSubData(GL_ARRAY_BUFFER, 0, N * sizeof(float3), &particles[0].pos);
glBindBuffer(GL_ARRAY_BUFFER, vbo_col);
glBufferSubData(GL_ARRAY_BUFFER, 0, N * sizeof(float3), &particles[0].col);
glBindVertexArray(0);

// Render particles
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glBindVertexArray(vao);
glDrawArrays(GL_POINTS, 0, N);
glBindVertexArray(0);

// Swap front and back buffers
glfwSwapBuffers(window);

// Poll for and process events
glfwPollEvents();
  
  
  
  
  
  
  }

// Clean up
cudaFree(d_positions);
cudaFree(d_velocities);
cudaFree(d_densities);
cudaFree(d_pressures);
cudaFree(d_forces);
cudaFree(d_colors);
glDeleteVertexArrays(1, &vao);
glDeleteBuffers(1, &vbo_pos);
glDeleteBuffers(1, &vbo_col);
glfwTerminate();

return 0;
}
  
