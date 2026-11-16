# esd-2026-workshop-g10
# Fuzz Testing Workshop: Participant Guide

ESD semester 7 · AFL++ and Docker

## The idea in 30 seconds

You will use a fuzzer called AFL++ to attack a small program you have not seen. The fuzzer throws thousands of odd inputs at it every second. Your job is to look at what breaks it and work out why.

Four words you will meet:

- **Docker container:** a ready-made mini computer on your laptop, with AFL++ already installed. You don't install anything yourself.
- **Seed:** a small, harmless starting input. AFL++ takes it and changes it again and again to make new inputs.
- **Crash:** the program stops with an error. Here the program calls `abort()` on purpose.
- **Minimize:** shrink a crashing input to the smallest version that still crashes the program.

## Before the workshop

1. Install and start Docker Desktop.
2. Clone the workshop repository.
3. From the repository root, open the workshop directory and build the image:

```bash
cd FuzzingWorkshop
docker compose build
```

The image is large and may take a while to download, so please do this before the session.

## Start the workshop container

On your computer, in the `FuzzingWorkshop/` folder, run:

```bash
docker compose run --rm workshop
```

Your terminal is now **inside the container**, in the folder `/workshop`. Every command below is typed there. First, set up AFL++ for Docker (copy and paste this once):

```bash
export AFL_SKIP_CPUFREQ=1 AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1 AFL_NO_AFFINITY=1
```

These three settings tell AFL++ not to complain about things that Docker doesn't allow. You can ignore what they mean.

## Exercise 1: Find the crash (20 minutes)

Work in pairs. **Do not read `exercises/participant/participant.c` until step 5.** The point is to work out the bug from the fuzzer's results, like a detective.

### Step 1: Compile the program

```bash
afl-cc exercises/participant/participant.c -o exercise
```

In plain words: this turns the C code into a program called `exercise`. AFL++'s own compiler (`afl-cc`) adds small markers inside it, so the fuzzer can see which parts of the code each input reaches.

### Step 2: Look at the seed

```bash
ls exercises/participant/inputs1
cat exercises/participant/inputs1/seed1.txt
```

In plain words: the seed is a small, harmless starting input that is already in the repository, so you don't have to create one. You should see the word `hello`. It does **not** crash the program. AFL++ takes it and changes it again and again to make new inputs. The first command lists the folder, and the second shows what is inside the seed file.

### Step 3: Run the fuzzer

```bash
afl-fuzz -i exercises/participant/inputs1 -o /tmp/out2 -- ./exercise
```

In plain words: `-i` is the folder with your seed (`inputs1`), `-o` is where AFL++ saves its results, and `./exercise` is the program to attack.

You will see a status screen. Watch the line **saved crashes**. When it shows 1 or more, press `Ctrl+C` to stop.

### Step 4: Look at the crashes and guess the rule

```bash
ls /tmp/out2/default/crashes
wc -c /tmp/out2/default/crashes/id:*
```

The first command lists the inputs that crashed the program. The second shows how many bytes (characters) each one has.

**Question for your pair: what do all the crashing inputs have in common? Write down your guess.** Hint: look at the numbers in the first column.

### Step 5: Check your guess

Now open `exercises/participant/participant.c` and read it. Were you right?

### Step 6: Minimize the crash

```bash
f=$(find /tmp/out2/default/crashes -type f -name 'id:*' | head -n 1)
afl-tmin -i "$f" -o /tmp/min -- ./exercise
wc -c /tmp/min
```

In plain words: pick one crashing file, then let `afl-tmin` remove everything that isn't needed. What is the smallest size that still crashes? Compare it with the sizes you saw in step 4.

Optional: replay the tiny crashing input yourself and check the exit code.

```bash
./exercise < /tmp/min; echo $?
```

The program should abort, and the number printed is the exit code (usually 134 for an abort).


## Check yourself

- What is the rule that makes the program crash?
- How did AFL++ find it without reading the code?
- Why is the minimized input more useful to a developer than the first crash file?

## Troubleshooting

- **`no configuration file provided`:** run Docker Compose inside `FuzzingWorkshop/`.
- **Terminal too small:** make the window at least 79 by 24.
- **No free CPU core:** check that `AFL_NO_AFFINITY=1` is set.
- **Output directory already exists:** use a new folder, for example `/tmp/out3`, and use that name in the later commands too.
- **Seed crashes immediately:** use a shorter seed that doesn't crash.
- **Docker not running:** start Docker Desktop before using Compose.

## Why this matters

The crashing input files are your evidence. A good bug report contains the input that causes the crash and the steps to reproduce it. That is exactly what you have after step 6: a tiny file and one command that makes the program fail every time.
 
