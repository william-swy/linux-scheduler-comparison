import pandas as pd
import matplotlib.pyplot as plt

filename = "cfs_multicore.tsv"

df = pd.read_csv(filename,  sep='\t', header=0)

plt.hist(df["wait"], bins=10000)