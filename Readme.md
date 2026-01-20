# LifeLine Operator's Side (PC demo) — Speech-to-Text Reimplementation (Attempt)

This project is an **attempt to reimplement the Life LineOperator Operator's Side** speech-to-text system, based on behavior/data from the **PC demo**. 

---

## Required Files

Place the following files alongside the build output:

- `lhsp/` (folder)
- `sony.lhk`
- `ss_wind32.plh`
- `lhcom02w.dll`
- `lhsp01.dll`
- `llus.grm`

---

## Build

### Requirements
- A C/C++ compiler (tested with **MSVC v14**, but others may work)

### Build Steps
```bash
make
```
# Notes
If you hit missing-DLL or file-not-found errors at runtime, double-check the Required Files section and paths.
Is expected that you see this error: "Error while opening the context (err 7: Specified language could not be found or license may be expired.)." this is where I stoped.

