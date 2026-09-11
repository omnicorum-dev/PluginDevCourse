# Episode 3 - Plugins and Best Practices

Task: 20260911-162745

## Topics to cover

- Buffers and buffer size
- Latency
- Multi-channel buffers (interleaved or not?)
- The amount of time we have to process the buffer
- Real-time safety
  - Absolutely nothing can be done unpredictably or occasionally slowly
  - no heap allocation
  - no locks
  - no blocking I/O
  - no logging
  - nothing non-deterministic on the audio thread
- Memory strategy for real-time code
  - Preallocate buffers and pools upfront (`prepareToPlay`)
- Processor vs. Editor
