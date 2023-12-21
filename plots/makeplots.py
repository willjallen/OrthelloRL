import numpy as np
import matplotlib.pyplot as plt
import json
from scipy.optimize import curve_fit

#Clunky extraction process for Elo/Iteration for A1,A2,B1,B2

with open(r"./arena/arena-4344.json") as file:
    file_contents = file.read()
data=json.loads(file_contents)

values_arrayA1 = list(data.values())
A1Dict={}
for item in values_arrayA1[1]:
    if item["model"]=='A1':
        A1Dict.update({item["iteration"]:item["ELO"]})

sorted_A1 = dict(sorted(A1Dict.items()))
A1Elo=[]
A1Iterations=[]
A1Elo=list(sorted_A1.values())
A1Iterations=list(sorted_A1.keys())


values_arrayA2 = list(data.values())
A2Dict={}
for item in values_arrayA2[1]:
    if item["model"]=='A2':
        A2Dict.update({item["iteration"]:item["ELO"]})

sorted_A2 = dict(sorted(A2Dict.items()))
A2Elo=[]
A2Iterations=[]
A2Elo=list(sorted_A2.values())
A2Iterations=list(sorted_A2.keys())


values_arrayB1 = list(data.values())
B1Dict={}
for item in values_arrayB1[1]:
    if item["model"]=='B1':
        B1Dict.update({item["iteration"]:item["ELO"]})

sorted_B1 = dict(sorted(B1Dict.items()))
B1Elo=[]
B1Iterations=[]
B1Elo=list(sorted_B1.values())
B1Iterations=list(sorted_B1.keys())

values_arrayB2 = list(data.values())
B2Dict={}
for item in values_arrayB2[1]:
    if item["model"]=='B2':
        B2Dict.update({item["iteration"]:item["ELO"]})

sorted_B2 = dict(sorted(B2Dict.items()))
B2Elo=[]
B2Iterations=[]
B2Elo=list(sorted_B2.values())
B2Iterations=list(sorted_B2.keys())

x1 = np.array(A1Iterations)
y1 = np.array(A1Elo)
# B2 model time ~ 9 minutes /itr
x2 = np.array(A2Iterations)
y2 = np.array(A2Elo)

x3 = np.array(B1Iterations)
y3 = np.array(B1Elo)

x4 = np.array(B2Iterations)
y4 = np.array(B2Elo)

#Truncating data after 160
idx = np.where(x1 == 160)[0][0]

# truncate all the x arrays at the index
x1 = x1[:idx+1]
x2 = x2[:idx]
x3 = x3[:idx+1]
x4 = x4[:idx+1]

# truncate all the y arrays at the index
y1 = y1[:idx+1]
y2 = y2[:idx]
y3 = y3[:idx+1]
y4 = y4[:idx+1]

#manually fixing x2,y2
x2=np.array([0,10,20,30,40,50,60,70,80,90,100,110,120,130,140,150,160])
y2=np.insert(y2,0,392.75839047)

print(x1)
print(y1)
print(x2)
print(y2)

#now setting the time arrays
z1=np.linspace(0,160*13,17)
z2=np.linspace(0,160*36,17)
z3=np.linspace(0,160*8,17)
z4=np.linspace(0,160*9,17)

# Fit polynomial of degree 4 to each set of data
p1 = np.polyfit(x1, y1, 4)
p2 = np.polyfit(x2, y2, 4)
p3 = np.polyfit(x3, y3, 4)
p4 = np.polyfit(x4, y4, 4)

# Generate predicted values for each set of data
x_pred1 = np.linspace(x1.min(), x1.max(), 50)
x_pred2 = np.linspace(x2.min(), x2.max(), 50)
x_pred3 = np.linspace(x3.min(), x3.max(), 50)
x_pred4 = np.linspace(x4.min(), x4.max(), 50)

y_pred1 = np.polyval(p1, x_pred1)
y_pred2 = np.polyval(p2, x_pred2)
y_pred3 = np.polyval(p3, x_pred3)
y_pred4 = np.polyval(p4, x_pred4)

# Calculate the corresponding confidence intervals for each set of data
sigma1 = np.sqrt(np.sum((y1 - np.polyval(p1, x1))**2)/(len(x1)-2))
conf_int1 = 1.96 * sigma1 * np.sqrt(1 + 1/len(x1) + (x_pred1 - np.mean(x1))**2/np.sum((x1 - np.mean(x1))**2))

sigma2 = np.sqrt(np.sum((y2 - np.polyval(p2, x2))**2)/(len(x2)-2))
conf_int2 = 1.96 * sigma2 * np.sqrt(1 + 1/len(x2) + (x_pred2 - np.mean(x2))**2/np.sum((x2 - np.mean(x2))**2))

sigma3 = np.sqrt(np.sum((y3 - np.polyval(p3, x3))**2)/(len(x3)-2))
conf_int3 = 1.96 * sigma3 * np.sqrt(1 + 1/len(x3) + (x_pred3 - np.mean(x3))**2/np.sum((x3 - np.mean(x3))**2))

sigma4 = np.sqrt(np.sum((y4 - np.polyval(p4, x4))**2)/(len(x4)-2))
conf_int4 = 1.96 * sigma4 * np.sqrt(1 + 1/len(x4) + (x_pred4 - np.mean(x4))**2/np.sum((x4 - np.mean(x4))**2))

fig1 = plt.figure(figsize=(8, 6))
axs1 = fig1.add_subplot()
axs1.scatter(x1, y1)
axs1.plot(x_pred1, y_pred1, color='b')
axs1.fill_between(x_pred1, y_pred1 - conf_int1, y_pred1 + conf_int1,
                  color='none', edgecolor='b', linewidth=2, dashes=(3,(3,2)))
axs1.set_title('A1 ELO Vs Iterations')
axs1.set_xlabel('Iterations')
axs1.set_ylabel('ELO')
plt.savefig("A1plot1a.svg")

fig2 = plt.figure(figsize=(8, 6))
axs2 = fig2.add_subplot()
axs2.scatter(x2, y2)
axs2.plot(x_pred2, y_pred2, color='c')
axs2.fill_between(x_pred2, y_pred2 - conf_int2, y_pred2 + conf_int2,
                  color='none', edgecolor='c', linewidth=2, dashes=(3,(3,2)))
axs2.set_title('A2 ELO Vs Iterations')
axs2.set_xlabel('Iterations')
axs2.set_ylabel('ELO')
plt.savefig("A2plot1b.svg")

fig3 = plt.figure(figsize=(8, 6))
axs3 = fig3.add_subplot()
axs3.scatter(x3, y3)
axs3.plot(x_pred3, y_pred3, color='r')
axs3.fill_between(x_pred3, y_pred3 - conf_int3, y_pred3 + conf_int3,
                  color='none', edgecolor='r', linewidth=2, dashes=(3,(3,2)))
axs3.set_title('B1 ELO Vs Iterations')
axs3.set_xlabel('Iterations')
axs3.set_ylabel('ELO')

plt.savefig("B1plot1c.svg")
fig4 = plt.figure(figsize=(8, 6))
axs4 = fig4.add_subplot()
axs4.scatter(x4, y4)
axs4.plot(x_pred4, y_pred4, color='#FF8C00')
axs4.fill_between(x_pred4, y_pred4 - conf_int4, y_pred4 + conf_int4,
                  color='none', edgecolor='#FF8C00', linewidth=2, dashes=(3,(3,2)))
axs4.set_title('B2 ELO Vs Iterations')
axs4.set_xlabel('Iterations')
axs4.set_ylabel('ELO')

plt.savefig("B2plot1d.svg")
#plt.show()
######################################################
######################################################
######################################################
fig, ax = plt.subplots()
zj1 = np.polyfit(z1, y1, 4)
zp1 = np.poly1d(zj1)

zj2 = np.polyfit(z2, y2, 4)
zp2 = np.poly1d(zj2)
z1_range = np.linspace(0, max(z1), 100)
z2_range = np.linspace(0, max(z2), 100)
# Plot both sets of data on the same graph
# plt.scatter(z1, y1, c='b', label='A1')
# plt.scatter(z2, y2, c='c', label='A2')
# Set the x-axis limits to match the range of x1
plt.xlim(-max(z2)/17, max(max(z1), max(z2)))
ax.plot(z1_range, zp1(z1_range), 'b-', label='Polyfit(B1)')
ax.plot(z2_range, zp2(z2_range), 'c-', label='Polyfit(B2)')
# Add axis labels and a legend
plt.xlabel('Time (minutes)')
plt.ylabel('ELO')
plt.legend(['A1', 'A2'], loc='lower right')

plt.title("A1&A2 ELO Vs Time")
plt.savefig("TA1A2plot1d.svg")
# Show the plot
plt.show()

fig, ax = plt.subplots()
zj3 = np.polyfit(z3, y3, 4)
zp3 = np.poly1d(zj3)

zj4 = np.polyfit(z4, y4, 4)
zp4 = np.poly1d(zj4)
z3_range = np.linspace(0, max(z3), 100)
z4_range = np.linspace(0, max(z4), 100)
# Plot both sets of data on the same graph
# plt.scatter(z3, y3, c='r', label='B1')
# plt.scatter(z4, y4, c='#FF8C00', label='B2')
# Set the x-axis limits to match the range of x1
plt.xlim(-max(z4)/17, max(max(z3), max(z4)))
ax.plot(z3_range, zp3(z3_range), 'r-', label='Polyfit(B1)')
ax.plot(z4_range, zp4(z4_range), 'y-', label='Polyfit(B2)')
# Add axis labels and a legend
plt.xlabel('Time (minutes)')
plt.ylabel('ELO')
plt.legend(['B1', 'B2'], loc='lower right')

plt.title("B1&B2 ELO Vs Time")
plt.savefig("TB1B2plot1d.svg")
# Show the plot
plt.show()

# ===========


# NO CONF-INT
fig, ax = plt.subplots()
zj1 = np.polyfit(z1, y1, 4)
zp1 = np.poly1d(zj1)

zj2 = np.polyfit(z2, y2, 4)
zp2 = np.poly1d(zj2)
z1_range = np.linspace(0, max(z1), 100)
z2_range = np.linspace(0, max(z2), 100)

zj3 = np.polyfit(z3, y3, 4)
zp3 = np.poly1d(zj3)

zj4 = np.polyfit(z4, y4, 4)
zp4 = np.poly1d(zj4)
z3_range = np.linspace(0, max(z3), 100)
z4_range = np.linspace(0, max(z4), 100)

plt.xlim(-max(z4)/17, max(max(z1), max(z4)))

ax.plot(z1_range, zp1(z1_range), color='b', label='Polyfit(B1)')
ax.plot(z2_range, zp2(z2_range), color='c', label='Polyfit(B2)')
ax.plot(z3_range, zp3(z3_range), color='r', label='Polyfit(B1)')
ax.plot(z4_range, zp4(z4_range), color='y', label='Polyfit(B2)')
# Add axis labels and a legend
plt.xlabel('Time (minutes)')
plt.ylabel('ELO')
plt.legend(['A1', 'A2', 'B1', 'B2'], loc='lower right')

plt.title("ELO Vs Time(All)")
plt.savefig("EloVTimeAll.svg")
# Show the plot
plt.show()

# CONF-INT



# ===========


fig, ax = plt.subplots()
zj1 = np.polyfit(z1, y1, 4)
zp1 = np.poly1d(zj1)

zj3 = np.polyfit(z3, y3, 4)
zp3 = np.poly1d(zj3)
z1_range = np.linspace(0, max(z1), 100)
z3_range = np.linspace(0, max(z3), 100)
# Plot both sets of data on the same graph
plt.scatter(z1, y1, c='b', label='A1')
plt.scatter(z3, y3, c='r', label='B1')
# Set the x-axis limits to match the range of x1
plt.xlim(-max(z3)/17, max(max(z1), max(z3)))
ax.plot(z1_range, zp1(z1_range), 'b-', label='Polyfit(A1)')
ax.plot(z3_range, zp3(z3_range), 'r-', label='Polyfit(B1)')
# Add axis labels and a legend
plt.xlabel('Time (minutes)')
plt.ylabel('ELO')
plt.legend(['A1', 'B1'], loc='lower right')
plt.savefig("TA1B1plot1d.svg")
plt.title("A1&B1 ELO Vs Time(Minutes)")

# Show the plot
plt.show()
fig, ax = plt.subplots()
zj2 = np.polyfit(z2, y2, 4)
zp2 = np.poly1d(zj2)

zj4 = np.polyfit(z4, y4, 4)
zp4 = np.poly1d(zj4)
z2_range = np.linspace(0, max(z2), 100)
z4_range = np.linspace(0, max(z4), 100)
# Plot both sets of data on the same graph
plt.scatter(z2, y2, c='c', label='A2')
plt.scatter(z4, y4, c='#FFC800', label='B2')
# Set the x-axis limits to match the range of x1
plt.xlim(-max(z2)/17, max(max(z2), max(z4)))
ax.plot(z2_range, zp2(z2_range), 'c-', label='Polyfit(A2)')
ax.plot(z4_range, zp4(z4_range), 'y-', label='Polyfit(B2)')
# Add axis labels and a legend
plt.xlabel('Time (minutes)')
plt.ylabel('ELO')
plt.legend(['A2', 'B2'], loc='lower right')

plt.title("A2&B2 ELO Vs Time(Minutes)")
plt.savefig("TA2B2plot1d.svg")
# Show the plot
plt.show()
######################################################
######################################################
######################################################
fig, ax = plt.subplots(figsize=(8, 6))

# Plot Just A1,A2
#ax.scatter(x1, y1, label='A1 Elo Vs Iterations')
ax.plot(x_pred1, y_pred1, label='A1', color='b')
ax.fill_between(x_pred1, y_pred1 - conf_int1, y_pred1 + conf_int1,
                  color='none', edgecolor='b', linewidth=2, dashes=(3,(3,2)))

#ax.scatter(x2, y2, label='A2 Elo Vs Iterations')
ax.plot(x_pred2, y_pred2, label='A2', color='c')
ax.fill_between(x_pred2, y_pred2 - conf_int2, y_pred2 + conf_int2,
                  color='none', edgecolor='c', linewidth=2, dashes=(3,(3,2)))

#ax.text(10, 120, 'Dotted Lines Represent Confidence Intervals, (95%)', fontsize=12, color='black')
ax.set_title('ELO Vs Iterations (A1&A2)')
ax.set_xlabel('Iterations')
ax.set_ylabel('ELO')
ax.legend(loc='lower right')

plt.savefig("A1A2plot.svg")
plt.show()

fig, ax = plt.subplots(figsize=(8, 6))

# Plot Just B1,B2
#ax.scatter(x3, y3, label='B1 Elo Vs Iterations')
ax.plot(x_pred3, y_pred3, label='B1', color='r')
ax.fill_between(x_pred3, y_pred3 - conf_int3, y_pred3 + conf_int3,
                  color='none', edgecolor='r', linewidth=2, dashes=(3,(3,2)))

#ax.scatter(x4, y4, label='B2 Elo Vs Iterations')
ax.plot(x_pred4, y_pred4, label='B2', color='#FF8C00')
ax.fill_between(x_pred4, y_pred4 - conf_int4, y_pred4 + conf_int4,
                  color='none', edgecolor='#FF8C00', linewidth=2, dashes=(3,(3,2)))

#ax.text(10, 120, 'Dotted Lines Represent Confidence Intervals, (95%)', fontsize=12, color='black')
ax.set_title('ELO Vs Iterations (B1&B2)')
ax.set_xlabel('Iterations')
ax.set_ylabel('ELO')
ax.legend(loc='lower right')

plt.savefig("B1B2plot.svg")
plt.show()

fig, ax = plt.subplots(figsize=(8, 6))

# Plot Just A1,B1
#ax.scatter(x1, y1, label='A1 Elo Vs Iterations')
ax.plot(x_pred1, y_pred1, label='A1', color='b')
ax.fill_between(x_pred1, y_pred1 - conf_int1, y_pred1 + conf_int1,
                  color='none', edgecolor='b', linewidth=2, dashes=(3,(3,2)))

#ax.scatter(x3, y3, label='B1 Elo Vs Iterations')
ax.plot(x_pred3, y_pred3, label='B1', color='r')
ax.fill_between(x_pred3, y_pred3 - conf_int3, y_pred3 + conf_int3,
                  color='none', edgecolor='r', linewidth=2, dashes=(3,(3,2)))

#ax.text(10, 120, 'Dotted Lines Represent Confidence Intervals, (95%)', fontsize=12, color='black')
ax.set_title('ELO Vs Iterations (A1&B1)')
ax.set_xlabel('Iterations')
ax.set_ylabel('ELO')
ax.legend(loc='lower right')

plt.savefig("A1B1plot.svg")
plt.show()

fig, ax = plt.subplots(figsize=(8, 6))

# Plot Just A2,B2
#ax.scatter(x2, y2, label='B2 Elo Vs Iterations')
ax.plot(x_pred4, y_pred4, label='B2', color='#FF8C00')
ax.fill_between(x_pred4, y_pred4 - conf_int4, y_pred4 + conf_int4,
                  color='none', edgecolor='#FF8C00', linewidth=2, dashes=(3,(3,2)))

#ax.scatter(x2, y2, label='A2 Elo Vs Iterations')
ax.plot(x_pred2, y_pred2, label='A2', color='c')
ax.fill_between(x_pred2, y_pred2 - conf_int2, y_pred2 + conf_int2,
                  color='none', edgecolor='c', linewidth=2, dashes=(3,(3,2)))

#ax.text(10, 120, 'Dotted Lines Represent Confidence Intervals, (95%)', fontsize=12, color='black')
ax.set_title('ELO Vs Iterations (A2&B2)')
ax.set_xlabel('Iterations')
ax.set_ylabel('ELO')
ax.legend(loc='lower right')

plt.savefig("A2B2plot.svg")
plt.show()

fig, ax = plt.subplots(figsize=(8, 6))

# Plot the scatterplots and polynomial best fits for each set of data
#ax.scatter(x1, y1, label='A1 Elo Vs Iterations')
ax.plot(x_pred1, y_pred1, label='A1', color='b')
ax.fill_between(x_pred1, y_pred1 - conf_int1, y_pred1 + conf_int1,
                  color='none', edgecolor='b', linewidth=2, dashes=(3,(3,2)))

#ax.scatter(x2, y2, label='A2 Elo Vs Iterations')
ax.plot(x_pred2, y_pred2, label='A2', color='c')
ax.fill_between(x_pred2, y_pred2 - conf_int2, y_pred2 + conf_int2,
                  color='none', edgecolor='c', linewidth=2, dashes=(3,(3,2)))

#ax.scatter(x3, y3, label='B1 Elo Vs Iterations')
ax.plot(x_pred3, y_pred3, label='B1', color='r')
ax.fill_between(x_pred3, y_pred3 - conf_int3, y_pred3 + conf_int3,
                  color='none', edgecolor='r', linewidth=2, dashes=(3,(3,2)))

#ax.scatter(x4, y4, label='B2 Elo Vs Iterations')
ax.plot(x_pred4, y_pred4, label='B2', color='#FF8C00')
ax.fill_between(x_pred4, y_pred4 - conf_int4, y_pred4 + conf_int4,
                  color='none', edgecolor='#FF8C00', linewidth=2, dashes=(3,(3,2)))

#ax.text(10, 120, 'Dotted Lines Represent Confidence Intervals, (95%)', fontsize=12, color='black')
ax.set_title('ELO Vs Iterations (All)')
ax.set_xlabel('Iterations')
ax.set_ylabel('ELO')
ax.legend(loc='lower right')

plt.savefig("Allplot.svg")
plt.show()


fig, ax = plt.subplots(figsize=(8, 6))

# Plot just the best-fit

ax.plot(x_pred1, y_pred1, color='b', label='A1')
ax.plot(x_pred2, y_pred2, color='c', label='A2')
ax.plot(x_pred3, y_pred3, color='r', label='B1')
ax.plot(x_pred4, y_pred4, color='y', label='B2')

ax.set_title('ELO Vs Iterations (All)')
ax.set_xlabel('Iterations')
ax.set_ylabel('ELO')
ax.legend()

plt.savefig("AllNoIntplot.svg")
plt.show()