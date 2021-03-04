import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("tests-out/1.out", header=None)

x30 = []
y30 = []
t30 = 0
x20 = []
y20 = []
t20 = 0
x10 = []
y10 = []
t10 = 0
for index, row in df.iterrows():
    tickets = int(row[2][9:])
    if (tickets == 30):
        x30.append(index)
        t30 += 1
        y30.append(t30)
    elif (tickets == 20):
        x20.append(index)
        t20 += 1
        y20.append(t20)
    elif (tickets == 10):
        x10.append(index)
        t10 += 1
        y10.append(t10)

# plot
plt.bar(x30, y30)
plt.bar(x20, y20)
plt.bar(x10, y10)
plt.margins(0)
plt.xlabel('time')
plt.ylabel('ticks')
plt.legend(['30 tickets', '20 tickets', '10 tickets'])
plt.savefig('ticks.png', dpi=300)
