from statistics import mean
num_iterations = 10
num_node_counts = ['1', '2', '4', '8']
num_processor_counts = ['1', '4', '16', '64', '256']
write_lines = []

# Build up values dict for population
vals = {}
for n in num_node_counts:
    vals.update({n:{}})
    for p in num_processor_counts:
        vals[n].update({p:{}})
        vals[n][p].update({'total': 0})

# Read in timing data
with open("./timings.txt", "r") as fp:
    lines = fp.readlines()

    for line in lines:
        if line[0] == 't':
            spl = line.split('_')
            nnodes = spl[2].strip('_')
            nprocs = spl[3].strip('_').strip()
        try:
            spl = line.split()
            for word in range(len(spl)):
                spl[word] = spl[word].rstrip(',')
            total = float(spl[-1])
            vals[nnodes][nprocs]['total'] += total
        except ValueError:
            pass
        except IndexError:
            pass

    for n in num_node_counts:
        for p in num_processor_counts:
            tot_mean = vals[n][p]['total'] / num_iterations
            if tot_mean != 0:
                write_lines.append(
                    f"NNODES = {n}\t"
                    f"NPROCS = {p}\t"
                    f"total = {round(tot_mean, 4)}")

# Write to a summary document
with open("./summary.txt", "w") as w_fp:
    for timing in write_lines:
        w_fp.write(timing + "\n")
        # w_fp.write(f"global sum: {str(sum)}\n\n")

print("Wrote summary.txt")
