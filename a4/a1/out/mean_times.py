from statistics import mean
num_iterations = 2
num_processor_counts = 6
error_offset = 2
write_lines = []
sums = []

# Read in timing data
with open("./timings.txt", "r") as fp:
    lines = fp.readlines()
    vals = []
    for line in lines:
        flt = 0.0
        try:
            flt = float(line.split()[-1])
            # print(flt)
            vals.append(flt)
        except ValueError:
            pass

    for group in range(0, len(vals), num_iterations):
        write_lines.append(
            f"NPROCS = {group * 2}, "
            f"mean = {round(mean(vals[group:group + num_iterations]), 4)}\n")

# Read in global sums
with open("./global_sums.txt", "r") as sum_fp:
    lines = sum_fp.readlines()
    for group in range(num_processor_counts):
        val = 0
        try:
            sums.append(int(lines[group * (num_iterations + 2) + error_offset].split()[-1]))
        except ValueError as err:
            pass

# Write to a summary document
with open("./summary.txt", "w") as w_fp:
    for (timing, sum) in zip(write_lines, sums):
        w_fp.write(timing)
        w_fp.write(f"global sum: {str(sum)}\n\n")

print("Wrote summary.txt")