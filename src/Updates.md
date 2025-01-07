# Changelog

## Version 0.0.1 - Initial Release

### Summary
This version includes the integration of the `ripser_short.h` header file with updates to the wrapper file (`ripserWrapper.cpp`) for compatibility and functionality improvements.

### Updates

#### Wrapper File (`ripserWrapper.cpp`)
- **Header Integration**: Replaced the old `ripser.h` include with `ripser_short.h`.
- **Function Name Updates**: Updated function calls in the wrapper file to align with the definitions provided in `ripser_short.h`. Specific updates include:
  - `getPointCloud()` and `getLowerDistMatrix()` are used for data conversion from R matrices.
  - `ripser_compute()` is now the primary function for computing persistence barcodes.
- **Streamlined Logic**: Removed redundant code for input handling and ensured modular structure for scalability.
- **Output Format**: Improved clarity in returned DataFrame for R users, detailing dimensions, birth times, and death times for persistence pairs.

## Version 0.0.2 - Task Division and Enhancements

### Updates
- **Task Comments**: Added detailed comments in `ripser_short.h` dividing floating-point operations and non-floating-point tasks between team members.
- **Collaboration Notes**: Clearly marked sections for Alice (floating-point tasks) and other team members (non-floating-point tasks).

### Notes
- Updated the header file to streamline the collaboration process and align with the new functionality.

## Version 0.0.3 - Additions to `ripserWrapper.cpp`

### Updates
- **Integer-Based Utilities**: Integrated logic for handling integer-based tasks such as managing binomial coefficients, verifying prime coefficients, and computing multiplicative inverses for modular arithmetic.
- **Union-Find Implementation**: Added a robust union-find structure for managing connected components during barcode computation.
- **Simplification Logic**: Introduced helper functions like `get_next_vertex()` and `get_simplex_vertices()` for efficient simplex enumeration.
- **Persistence Computation**: Enhanced functionality for computing persistence pairs by incorporating additional structures for managing diameter entries and coboundary enumerators.
- **Input Handling Improvements**: Reused modularized functions for converting R matrix inputs into suitable distance matrix formats.
- **Output Clarifications**: Maintained the modular structure to ensure outputs remain clear and consistent for R users while adding critical functionality.

### Notes
- Ensured seamless integration between Kent’s original code and Sean’s additions for clarity and performance.
