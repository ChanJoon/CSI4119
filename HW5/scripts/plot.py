import matplotlib.pyplot as plt

# Dictionary to hold your data
data = {
    "reduce1": {64: 0.030720, 4194304: 0.219488, 8388608: 0.428800, 16777216: 0.797888},
    "reduce2": {64: 0.030720, 4194304: 0.156864, 8388608: 0.288928, 16777216: 0.521888},
    "reduce3": {64: 0.032768, 4194304: 0.137632, 8388608: 0.272896, 16777216: 0.498144},
    "reduce4": {64: 0.031744, 4194304: 0.078784, 8388608: 0.194208},
    "reduce5": {64: 0.031808, 4194304: 0.053440, 8388608: 0.095872, 16777216: 0.132256},
    "reduce6": {64: 0.028672, 4194304: 0.057536, 8388608: 0.101600, 16777216: 0.134016},
    "reduce7": {64: 0.031744, 4194304: 0.055200, 8388608: 0.091680, 16777216: 0.137024},
}

plt.figure(figsize=(10, 8))

# Plot each series
for name, timings in data.items():
    sizes = list(timings.keys())
    times = list(timings.values())
    plt.plot(sizes, times, marker='o', label=name)

plt.xscale('log')
plt.yscale('log')

plt.xlabel('Size of Sequence')
plt.ylabel('Time (msec)')
plt.title('Performance Comparison of Reduction Algorithms')
plt.legend()
plt.grid(True)

plt.show()
