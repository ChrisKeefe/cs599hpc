from statistics import mean
num_iterations = 10
num_processor_counts = ['1']
write_lines = []

# Build up values dict for population
vals = {}
for p in num_processor_counts:
    vals.update({p:{}})
    vals[p].update({'total': 0})

# Read in timing data
with open("./timings.txt", "r") as fp:
    lines = fp.readlines()

    for line in lines:
        if line[0] == 'c':
            nprocs = line[6:].strip()

        try:
            spl = line.split()
            for word in range(len(spl)):
                spl[word] = spl[word].rstrip(',')
            total = float(spl[-1])
            vals[nprocs]['total'] += total
        except ValueError:
            pass
        except IndexError:
            pass

    for p in num_processor_counts:
        tot_mean = vals[p][k]['total'] / num_iterations
        write_lines.append(
            f"NPROCS = {p}\t"
            f"total = {round(tot_mean, 4)}")

# Write to a summary document
with open("./summary.txt", "w") as w_fp:
    for timing in write_lines:
        w_fp.write(timing + "\n")
        # w_fp.write(f"global sum: {str(sum)}\n\n")

print("Wrote summary.txt")
