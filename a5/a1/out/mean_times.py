from statistics import mean
num_iterations = 3
num_processor_counts = 5
error_offset = 0
write_lines = []
sums = []

# Read in timing data
with open("./timings.txt", "r") as fp:
    lines = fp.readlines()
    vals = []
    for line in lines:
        flt = 0.0
        try:
            spl = line.split()
            for word in range(len(spl)):
                spl[word] = spl[word].rstrip(',')
            build = float(spl[3])
            query = float(spl[5])
            el = float(spl[-1])
            vals.append((build, query, el))
        except ValueError:
            pass
        except IndexError:
            pass

    for i, group_st in enumerate(range(0, len(vals), num_iterations)):
        group_ctr = 0
        if group_st == 0:
            nprocs = 1
        else:
            nprocs = 4 * i
        group_end = group_st + num_iterations
        grouples = vals[group_st: group_end]
        build_mean = 0
        que_mean = 0
        tot_mean = 0
        for i in range(num_iterations):
            build_mean += grouples[i][0]
            que_mean += grouples[i][1]
            tot_mean += grouples[i][2]
        build_mean /= num_iterations
        que_mean /= num_iterations
        tot_mean /= num_iterations
        write_lines.append(
            f"NPROCS = {nprocs}, "
            f"build = {round(build_mean, 4)}, "
            f"query = {round(que_mean, 4)}, "
            f"total = {round(tot_mean, 4)}, ")
        print(write_lines)

# Read in global sums
with open("./global_sums.txt", "r") as sum_fp:
    lines = sum_fp.readlines()
    sums = []
    for group in range(num_processor_counts):
        try:
            sums.append(int(lines[group * (num_iterations + 1) + 1].split()[-1]))
        except ValueError as err:
            pass
    print(sums)

# Write to a summary document
with open("./summary.txt", "w") as w_fp:
    for (timing, sum) in zip(write_lines, sums):
        w_fp.write(timing)
        w_fp.write(f"global sum: {str(sum)}\n\n")

print("Wrote summary.txt")
