import csv
from scipy.spatial import distance_matrix

print("DM computed from assignment sample data")
dm = [[1.0,2.0,3.0,4.0],
      [6.0,7.0,8.0,9.0],
      [11.0,12.0,13.0,14.0],
      [15.0,16.0,17.0,18.0]]
print(distance_matrix(dm, dm))

print("###############################")
print("DM from first three rows:")
rows = 3
with open('../MSD_year_prediction_normalize_0_1_100k.txt') as fh:
    reader = csv.reader(fh, quoting=csv.QUOTE_NONNUMERIC)
    lines = []
    for row_num in range(rows):
        lines.append(next(reader))

    print("N_Rows:", len(lines))
    print("DIMS:", len(lines[0]))
    print(distance_matrix(lines, lines))

