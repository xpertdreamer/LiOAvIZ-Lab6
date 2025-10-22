# LiOAvIZ-Lab6

# Core Implementation Architecture

## 🎲 Random Generation System

**What it does**: Creates predictable random graphs with controlled edge probabilities.

**How it works under the hood**:
- Uses a **Linear Congruential Generator (LCG)** - a simple but effective random number algorithm
- The magic numbers `1664525` and `1013904223` are well-known constants that produce good random sequences
- The `& 0x7fffffff` part ensures we only get positive numbers (bitmask trick)

**Seeding strategy**:
- If you provide `seed=0`, it uses the current time in nanoseconds plus an internal counter
- This means: same parameters + same seed = identical graph every time
- The static counter ensures that even if you create multiple graphs at the same nanosecond, they'll be different

**Thread safety note**: The `static counter++` is not thread-safe. If you use this from multiple threads, you'll need synchronization.

## 💾 Memory Management

**The dual approach**:
1. **Raw pointers for matrices**: `int** adj_matrix` uses manual `new[]` and `delete[]`
2. **Smart pointers for graph objects**: `std::unique_ptr<Graph>` in the adapter layer

**Why this mix?**
- Matrices need precise control over 2D array allocation
- Graph objects benefit from RAII (automatic cleanup when they go out of scope)

**The cleanup dance**:
```cpp
// This happens in a specific order:
1. Delete each row: delete[] graph.adj_matrix[i]
2. Delete the column pointers: delete[] graph.adj_matrix  
3. Set to nullptr to prevent double-free
4. Clear the adjacency list
5. Reset the vertex count
```

**Memory leak prevention**: The destructor `~GraphConsoleAdapter()` calls `cleanup()` which ensures all graphs are properly deleted, even if someone forgets to call cleanup manually.

## 🌐 Cross-Platform Compatibility

**The challenge**: Windows, Linux, and macOS handle terminals differently, especially colors and screen clearing.

**Color solution**:
- On Windows 10+, we enable **Virtual Terminal Processing** which lets Windows understand ANSI color codes
- On Linux/macOS, ANSI codes just work out of the box
- We maintain a mapping: `"error" → "red" → "\033[31m"`

**Config file finding**:
- Windows looks in `%APPDATA%/graph_console/`
- Linux/macOS looks in `~/.config/graph_console/`
- Also checks common development paths: `./resources/config_files/`, `../config/`, etc.
- This means your config files will be found whether you're running from IDE, terminal, or installed app

**Screen clearing**: Simple `system("cls")` vs `system("clear")` wrapper that abstracts the platform difference.

## 🚨 Error Handling Philosophy

**Defensive programming throughout**:

**Input validation at every layer**:
```cpp
// Before doing anything with vertices:
if (v >= target->n || v < 0 || u >= target->n || u < 0 || v == u) {
    std::cout << "Invalid vertex number" << std::endl;
    return;  // Fail fast, don't crash
}
```

**Exception safety**:
- We use RAII so even if an exception is thrown, memory gets cleaned up
- Each command handler has its own try-catch block
- The main loop has a global catch-all for unexpected exceptions

**User-friendly errors**:
- Colors help distinguish errors (red) from warnings (yellow) from success (green)
- Error messages explain what went wrong and often suggest fixes

## 🔧 Graph Operation Internals

**The dual representation challenge**:
We maintain both adjacency matrix AND adjacency list. When we modify one, we have to update the other.

**Vertex identification flow**:
1. Choose which vertex to keep (the smaller index)
2. Merge edges: keep gets edges from both vertices
3. Handle self-loops specially
4. Create new smaller matrix without the removed vertex
5. Update adjacency lists to match
6. Renumber all vertex indices greater than the removed one

**Matrix resizing trick**:
We can't actually resize arrays, so we:
1. Allocate a new smaller matrix
2. Copy everything except the removed vertex
3. Delete the old matrix
4. Point to the new matrix

**Edge contraction vs identification**:
- **Identify**: Merge any two vertices (they don't need to be connected)
- **Contract**: Merge two vertices that MUST have an edge between them

## 🎮 Command System Architecture

**The handler pattern**:
```cpp
// We store commands in a map: name → CommandInfo
// CommandInfo contains: function pointer, description, parameters, usage
```

**How input flows**:
1. User types `"create 5 0.5 0.1"`
2. `tokenize()` splits into `["create", "5", "0.5", "0.1"]`
3. Look up `"create"` in commands map
4. Extract arguments `["5", "0.5", "0.1"]`
5. Call the registered lambda with those arguments

**Alias resolution**:
- We maintain a separate `aliases` map
- `"new" → "create"`, `"show" → "print"`
- Before command lookup, we check if the input is an alias and replace it

**History management**:
- Uses a `deque` (double-ended queue) with fixed size
- New commands go to the front, old ones fall off the back
- This is more efficient than a vector for our use case

## ⚙️ Configuration System

**INI-style parsing**:
- We parse `key = value` pairs
- Sections are in `[brackets]`
- `#` starts a comment
- We handle boolean values intelligently: `"true"`, `"1"`, `"yes"`, `"on"` all mean true

**The search path strategy**:
We try multiple locations in order:
1. Current directory
2. `./resources/config_files/`
3. `../resources/config_files/` (common when building in separate build/ directory)
4. Platform-specific user config directory
5. Finally, if nothing found, we use a default location

This means developers can keep config files in their source tree, but installed versions use proper system directories.

**Color system abstraction**:
We don't hardcode ANSI sequences. Instead we have:
```
"error_color" = "red" (in config)
"red" = "\033[31m" (in internal map)
```
This lets users customize colors without understanding ANSI codes.

## 🏗️ Architectural Decisions

**Why both matrix and list?**
- Matrices are fast for edge existence checks: `O(1)` 
- Lists are memory-efficient for sparse graphs
- Some operations are easier with one representation vs the other

**Why the console adapter pattern?**
- Separates graph logic from UI logic
- Makes it easy to replace console with GUI later
- Graph operations can be tested without user interaction

**Manual memory management in core, smart pointers in adapter**:
- Core library gives maximum control for complex graph operations
- Adapter layer provides safety and convenience for application use
- Best of both worlds: performance + safety

**The static counter in random generation**:
- Ensures uniqueness when seeding from time
- Simple solution for single-threaded use
- Would need `std::atomic` for thread safety

This architecture balances performance, safety, and maintainability while providing a solid foundation for graph algorithm experimentation.

## Build with CLion
