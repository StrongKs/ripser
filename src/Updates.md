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

#### Header File (`ripser_short.h`)
- Integrated support for point cloud and distance matrix computations.
- Enhanced performance with optimized helper functions for computing barcodes.
- Maintained compatibility with the wrapper for seamless interaction with R.

### Notes
- This release introduces compatibility with both point cloud and distance matrix inputs, ensuring flexibility for various data formats.
- All changes align with the structure and functionality of `ripser_short.h` to ensure a clean and efficient implementation.
- Focused on enabling future extensions by maintaining a modular and flexible codebase.

---

### Next Steps
- Gather user feedback for further improvements.
- Evaluate additional features, such as parameter validation and advanced input formats.
