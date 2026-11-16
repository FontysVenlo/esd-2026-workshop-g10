# Fuzz Testing Workshop — Participant Guide

ESD semester 7 · AFL++ and Docker

## Before the workshop

1. Install and start Docker Desktop.
2. Clone the workshop repository.
3. From the repository root, open the workshop directory and build the image:

```bash
cd FuzzingWorkshop
docker compose build
```

> The image may take time to download. Please build it before the session.

## Start the workshop container

From `FuzzingWorkshop/` on your computer:

```bash
docker compose run --rm workshop
```

The following commands run **inside the container** at `/workshop`:

```bash
export AFL_SKIP_CPUFREQ=1 AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1 AFL_NO_AFFINITY=1
```

## Exercise 1 — Become the fuzzer (pairs)

Think of unusual inputs for a username and age form. Consider **format**, **length**, and **boundary values**. Record any unexpected behavior and be ready to share it.

## Live demo — Watch AFL++ find a crash

Follow along as the presenters demonstrate:

```bash
afl-cc exercises/demo/demo.c -o demo
mkdir -p /tmp/in
printf 'A' > /tmp/in/seed
afl-fuzz -i /tmp/in -o /tmp/out -- ./demo
```

Stop AFL++ with `Ctrl+C` after it finds a crash. Inspect and replay it:

```bash
ls /tmp/out/default/crashes
f=$(find /tmp/out/default/crashes -type f -name 'id:*' | head -n 1)
od -c "$f"
./demo < "$f"
echo $?
```

## Exercise 2 — Find the crash (20 minutes)

**Do not read `exercises/participant/participant.c` until step 5.**

1. Compile the program:

   ```bash
   afl-cc exercises/participant/participant.c -o exercise
   ```

2. Create a harmless seed:

   ```bash
   mkdir -p /tmp/in2
   printf 'hi' > /tmp/in2/seed
   ```

3. Run AFL++ and stop with `Ctrl+C` once a crash is saved:

   ```bash
   afl-fuzz -i /tmp/in2 -o /tmp/out2 -- ./exercise
   ```

4. Inspect the saved crashing input. **What do you think makes the program crash?**

   ```bash
   ls /tmp/out2/default/crashes
   wc -c /tmp/out2/default/crashes/id:*
   ```

5. Open `exercises/participant/participant.c` and check your hypothesis.

6. Minimize the crash and compare the sizes:

   ```bash
   f=$(find /tmp/out2/default/crashes -type f -name 'id:*' | head -n 1)
   afl-tmin -i "$f" -o /tmp/min -- ./exercise
   wc -c /tmp/min
   ```

7. **Stretch:** change the crash condition, recompile and fuzz with a fresh output folder; or try a different seed and compare how quickly AFL++ finds a crash.

## Troubleshooting

- **`no configuration file provided`**: run Docker Compose inside `FuzzingWorkshop/`.
- **Terminal too small**: resize to at least 79×24.
- **No free CPU core**: check that `AFL_NO_AFFINITY=1` is set.
- **Output directory already exists**: use a new output directory, e.g. `/tmp/out3`, and adjust subsequent paths accordingly.
- **Seed crashes immediately**: use a shorter, non-crashing seed.
- **Docker not running**: start Docker Desktop before using Compose.

The crashing input files and their contents are evidence: save the input and describe how to reproduce the crash in a bug report.
