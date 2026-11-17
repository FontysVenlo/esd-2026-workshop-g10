# Demo Commands – Francesco

## Step 1: Start the container

```bash
cd FuzzingWorkshop
docker compose run --rm workshop
```

## Step 2: Settings

```bash
export AFL_SKIP_CPUFREQ=1 AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1 AFL_NO_AFFINITY=1
```

## Step 3: Show the code

```bash
cat exercises/demo/demo.c
```

## Step 4: Compile with AFL++

```bash
afl-cc exercises/demo/demo.c -o demo
```

## Step 5: Create a seed

```bash
mkdir -p /tmp/in
printf 'A' > /tmp/in/seed
```

## Step 6: Run the fuzzer

```bash
afl-fuzz -i /tmp/in -o /tmp/out -- ./demo
```

**Stop with Ctrl + C** once "saved crashes" shows 1.

## Step 7: Look at the result

```bash
ls /tmp/out/default/crashes
```

```bash
f=$(find /tmp/out/default/crashes -type f -name 'id:*' | head -n 1)
od -c "$f"
```

## Step 8: Replay the crash

```bash
./demo < "$f"
echo $?
```

**Expected result:** The program aborts, and the exit code is `134`.
