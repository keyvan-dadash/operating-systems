import matplotlib.pyplot as plt
import numpy as np
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import math

file1 = open('data', 'r') 
Lines = file1.readlines()
  
file1.close()


data = {}
pages = []
times = []
count = 0
big_page = 0
big_time = -1
for line in Lines: 
    splitted = line.strip().split("  ")
    data[splitted[0]] = splitted[2]
    pages.append(float(splitted[0]))
    times.append(float(splitted[2]))
    # plt.plot(float(splitted[0]), float(splitted[2]), 'o-')
    big_page = float(splitted[0])
    if (float(splitted[2]) > big_time):
        big_time = float(splitted[2])
    
# plt.title('A tale of 2 subplots')
# plt.ylabel('Damped oscillation')

# plt.show()

fig, ax = plt.subplots(figsize=(10, 6))

ax.plot(pages, 
        times)

        
# df = pd.DataFrame.from_dict(data,orient='index',columns=['Revenue'])
# dg =pd.to_numeric(df['Revenue'])
# dc = pd.Series(dg.index.values.tolist()).to_frame('Company') 
# dat = df.assign(Company=dc.values)

# data = dat.sort_values(by=['Revenue'])
 
# plt.scatter(data['Revenue'],data['Company'])
# plt.grid()
plt.xscale("log")
plt.show()