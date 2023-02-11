from matplotlib import pyplot as plt
import pandas as pd

################# 90% large  low cont #############################
h1_1=pd.read_csv("runs/hashmap-hirocap-lowcon-u10-p8tm-psi-v2-1", sep="\t")
h1_2=pd.read_csv("runs/hashmap-hirocap-lowcon-u10-p8tm-psi-v2-qfi-1", sep="\t")
h1_3=pd.read_csv("runs/hashmap-hirocap-lowcon-u10-p8tm-psi-v2-fi-1", sep="\t")
################# 90% large high cont #############################

################# 50% ro low cont ##################################
h2_1=pd.read_csv("runs/hashmap-hirocap-lowcon-u50-p8tm-psi-v2-1", sep="\t")
h2_2=pd.read_csv("runs/hashmap-hirocap-lowcon-u50-p8tm-psi-v2-qfi-1", sep="\t")
h2_3=pd.read_csv("runs/hashmap-hirocap-lowcon-u50-p8tm-psi-v2-fi-1", sep="\t")
################# 50% ro high cont #################################

################# 10% ro low cont ###############################
h3_1=pd.read_csv("runs/hashmap-hirocap-lowcon-u90-p8tm-psi-v2-1", sep="\t")
h3_2=pd.read_csv("runs/hashmap-hirocap-lowcon-u90-p8tm-psi-v2-qfi-1", sep="\t")
h3_3=pd.read_csv("runs/hashmap-hirocap-lowcon-u90-p8tm-psi-v2-fi-1", sep="\t")
################# 90% small high cont ##############################

####################################################################
threads= h1_1['threads']

h1=[h1_1, h1_2, h1_3]
h2=[h2_1, h2_2, h2_3]
h3=[h3_1, h3_2, h3_3]
h=[h1,h2,h3]

tx_h1=[0, 0, 0]
tx_h2=[0, 0, 0]
tx_h3=[0, 0, 0]

std_h1=[0, 0, 0]
std_h2=[0, 0, 0]
std_h3=[0, 0, 0]

for i in range(3):
    tx_h1[i]=h1[i]['Throughput_avg']/100000
    tx_h2[i]=h2[i]['Throughput_avg']/100000
    tx_h3[i]=h3[i]['Throughput_avg']/100000
    std_h1[i]=h1[i]['Throughput_std']/100000
    std_h2[i]=h2[i]['Throughput_std']/100000
    std_h3[i]=h3[i]['Throughput_std']/100000



tx_h=[tx_h1, tx_h2, tx_h3]
std_h=[std_h1, std_h2, std_h3]

#print(h2_1['rot_trans_avg'])

fig1, ax1 = plt.subplots(nrows=3,ncols=4)
fig2, ax2 = plt.subplots(nrows=3,ncols=4)
fig3, ax3 = plt.subplots(nrows=3,ncols=4)
title=["PSI","PSI qf inside sus/res","PSI same as above +\nflush inside qf "]
title2=["Hash-map 90% read-only txs low contention","Hash-map 50% read-only txs low contention","Hash-map 10% read-only txs low contention"]
ax=[ax1,ax2,ax3]

x=('1t','2t','4t','8t','16t','32t')

for j in range(3):
    for i in range(3):
        ax[j][1,i].bar(x,h[j][i]['rot_nontrans_avg'],label="Non-Transactional")
        ax[j][1,i].bar(x,h[j][i]['rot_trans_avg'],bottom=h[j][i]['rot_nontrans_avg'],label="Transactional")
        ax[j][1,i].bar(x,h[j][i]['rot_capacity_avg'],bottom=h[j][i]['rot_nontrans_avg']+h[j][i]['rot_trans_avg'],label="capacity")
        ax[j][1,i].bar(x,h[j][i]['rot_user_avg'],bottom=h[j][i]['rot_nontrans_avg']+h[j][i]['rot_trans_avg']+h[j][i]['rot_capacity_avg'],label="user abort")
        #ax[j][1,i].bar(x,h[j][i]['rot_self_avg'],bottom=h[j][i]['rot_nontrans_avg']+h[j][i]['rot_trans_avg']+h[j][i]['rot_capacity_avg'],label="self-abort")
        ax[j][1,i].set_title(title[i])
        ax[j][1,0].legend(loc='upper left',bbox_to_anchor=(-0.75, 1))
        ax[j][1,i].set_xlabel("Threads")
        ax[j][1,i].set_ylabel("Aborts (%)")
##############################
for j in range(3):
    for i in range(3):
        ax[j][2,i].bar(x,h[j][i]['gl_commits_avg'],label="SGL")
        #ax[j][2,i].bar(x,h[j][i]['rot_commits_avg'],bottom=h[j][i]['gl_commits_avg'],label="ROT")
        ax[j][2,i].set_title(title[i])
        ax[j][2,i].legend(loc='upper left')
        ax[j][2,i].set_xlabel("Threads")
        ax[j][2,i].set_ylabel("Tx (%)")
#######################################

for j in range(3):
    for i in range(3):
        ax[j][0,0].errorbar(x,tx_h[j][i],  yerr=std_h[j][i], capsize=5, color="C"+str(i))
        #ax[j][0,0].scatter(x,tx_h[j][i])
        ax[j][0,0].plot(x,tx_h[j][i], label=title[i],color="C"+str(i))
    ax[j][0,0].set_title(title2[j])
    ax[j][0,0].legend(loc='upper left',bbox_to_anchor=(-0.75, 1.35))
    ax[j][0,0].set_ylabel("Throughput (10$^5$)")
    ax[j][0,0].set_xlabel("Threads")

for j in range(3):
    for i in range(3):
        ax[j][0,1].errorbar(x,h[j][i]["sus_time_avg"],  yerr=h[j][i]["sus_time_std"], capsize=5, color="C"+str(i))
        #ax[j][0,1].scatter(x,tx_h[j][i])
        #ax[j][0,1].plot(x,h[j][i]["sus_time_avg"], label=title[i],color="C"+str(i))
    ax[j][0,1].set_title("Accumulated Cpu cycles in sus/res",y=1.05)
    #ax[j][0,1].legend(loc='upper left',bbox_to_anchor=(-0.75, 1.1))
    ax[j][0,1].set_ylabel("Accumulated Cpu cycles")
    ax[j][0,1].set_xlabel("Threads")

for j in range(3):
    for i in range(3):
        ax[j][0,2].errorbar(x,h[j][i]["flush_time_avg"],  yerr=h[j][i]["flush_time_std"], capsize=5, color="C"+str(i))
        #ax[j][0,1].scatter(x,tx_h[j][i])
        #ax[j][0,1].plot(x,h[j][i]["sus_time_avg"], label=title[i],color="C"+str(i))
    ax[j][0,2].set_title("Accumulated Cpu cycles in\nFlushing After commit",y=1.05)
    #ax[j][0,2].legend(loc='upper left',bbox_to_anchor=(-0.75, 1.1))
    ax[j][0,2].set_ylabel("Accumulated Cpu cycles")
    ax[j][0,2].set_xlabel("Threads")

for j in range(3):
    for i in range(3):
        ax[j][0,3].errorbar(x,h[j][i]["wait2_time_avg"],  yerr=h[j][i]["wait2_time_std"], capsize=5, color="C"+str(i))
        #ax[j][0,1].scatter(x,tx_h[j][i])
        #ax[j][0,1].plot(x,h[j][i]["sus_time_avg"], label=title[i],color="C"+str(i))
    ax[j][0,3].set_title("Accumulated Cpu cycles in\nWaiting after commit",y=1.05)
    #ax[j][0,2].legend(loc='upper left',bbox_to_anchor=(-0.75, 1.1))
    ax[j][0,3].set_ylabel("Accumulated Cpu cycles")
    ax[j][0,3].set_xlabel("Threads")


for j in range(3):
    for i in range(3):
        ax[j][1,3].errorbar(x,h[j][i]["wait_time_avg"],  yerr=h[j][i]["wait_time_std"], capsize=5, color="C"+str(i))
        #ax[j][0,1].scatter(x,tx_h[j][i])
        #ax[j][0,1].plot(x,h[j][i]["sus_time_avg"], label=title[i],color="C"+str(i))
    ax[j][1,3].set_title("Accumulated Cpu cycles in\nqf",y=1.05)
    #ax[j][0,2].legend(loc='upper left',bbox_to_anchor=(-0.75, 1.1))
    ax[j][1,3].set_ylabel("Accumulated Cpu cycles")
    ax[j][1,3].set_xlabel("Threads")

for j in range(3):
    for i in range(3):
        ax[j][2,3].errorbar(x,h[j][i]["Total_time_avg"],  yerr=h[j][i]["Total_time_std"], capsize=5, color="C"+str(i))
        #ax[j][0,1].scatter(x,tx_h[j][i])
        #ax[j][0,1].plot(x,h[j][i]["sus_time_avg"], label=title[i],color="C"+str(i))
    ax[j][2,3].set_title("Total time",y=1.05)
    #ax[j][0,2].legend(loc='upper left',bbox_to_anchor=(-0.75, 1.1))
    ax[j][2,3].set_ylabel("Accumulated Cpu cycles")
    ax[j][2,3].set_xlabel("Threads")

#
#
##tx.set_xlabel("Threads")
##tx.set_ylabel("Throughput (10$^5$)")
##tx.set_xlabel("Threads")
##
##
##tx.legend(loc='upper left')
##tx.set_title("90% read only, Low contention")
#
#
#plt.yscale('linear')
#
#
##fig.tight_layout()
##fig1.tight_layout()
##fig2.tight_layout()
##fig3.tight_layout()
##fig4.tight_layout()
##fig5.tight_layout()
##fig6.tight_layout()
##fig.subplots_adjust( left  = 0.125, right = 0.85, bottom = 0.1, top = 0.9, wspace = 0.25, hspace = 0.35)
fig1.subplots_adjust(left  = 0.125, right = 0.9, bottom = 0.1, top = 0.9, wspace = 0.25, hspace = 0.58)
fig2.subplots_adjust(left  = 0.125, right = 0.9, bottom = 0.1, top = 0.9, wspace = 0.25, hspace = 0.58)
fig3.subplots_adjust(left  = 0.125, right = 0.9, bottom = 0.1, top = 0.9, wspace = 0.25, hspace = 0.58)
#fig4.subplots_adjust(left  = 0.125, right = 0.9, bottom = 0.1, top = 0.9, wspace = 0.25, hspace = 0.58)
#fig5.subplots_adjust(left  = 0.125, right = 0.9, bottom = 0.1, top = 0.9, wspace = 0.25, hspace = 0.58)
#fig6.subplots_adjust(left  = 0.125, right = 0.9, bottom = 0.1, top = 0.9, wspace = 0.25, hspace = 0.58)
#
#ax1[2,3].remove()
#ax2[2,3].remove()
#ax3[2,3].remove()
#ax4[1,2].remove()
#ax5[1,2].remove()
#ax6[1,2].remove()
#
#
plt.show()