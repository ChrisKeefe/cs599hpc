from statistics import mean
num_iterations = 3
num_processor_counts = ['24', '28', '32', '36', '40']
num_k = ['2', '25', '50', '100']
write_lines = []
sums = []

# Build up values dict for population
vals = {}
for p in num_processor_counts:
    vals.update({p:{}})
    for k in num_k:
        vals[p].update({k: {}})
        vals[p][k].update({'distance': 0})
        vals[p][k].update({'centroid': 0})
        vals[p][k].update({'total': 0})

# Read in timing data
with open("./timings.txt", "r") as fp:
    lines = fp.readlines()

    for line in lines:
        if line[0] == 'k':
            short = line[6:].strip()
            nprocs, k = short.split(sep='_')

        try:
            spl = line.split()
            for word in range(len(spl)):
                spl[word] = spl[word].rstrip(',')
            distance = float(spl[4])
            centroid = float(spl[-1])
            total = float(spl[2])
            vals[nprocs][k]['distance'] += distance
            vals[nprocs][k]['centroid'] += centroid
            vals[nprocs][k]['total'] += total
        except ValueError:
            pass
        except IndexError:
            pass

    for p in num_processor_counts:
        for k in num_k:
            d_mean = vals[p][k]['distance'] / num_iterations
            c_mean = vals[p][k]['centroid'] / num_iterations
            tot_mean = vals[p][k]['total'] / num_iterations
            write_lines.append(
                f"NPROCS = {p}\t"
                f"K = {k}\t"
                f"dist = {round(d_mean, 4)}\t"
                f"centroid = {round(c_mean, 4)}\t"
                f"total = {round(tot_mean, 4)}")

# Write to a summary document
with open("./summary.txt", "w") as w_fp:
    for timing in write_lines:
        w_fp.write(timing + "\n")
        # w_fp.write(f"global sum: {str(sum)}\n\n")

print("Wrote summary.txt")
