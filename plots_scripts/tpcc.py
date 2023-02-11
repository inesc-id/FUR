from matplotlib import pyplot as plt
import pandas as pd

################# 90% large  low cont #############################
h1_1=pd.read_csv("runs/tpcc-std-w32-p8tm-si-v2-1", sep="\t")
h1_2=pd.read_csv("runs/tpcc-std-w32-p8tm-psi-v2-1", sep="\t")
h1_3=pd.read_csv("runs/tpcc-std-w32-p8tm-psi-v2-fi-1", sep="\t")
h1_4=pd.read_csv("runs/tpcc-std-w32-htm-sgl-2", sep="\t")
h1_5=pd.read_csv("runs/tpcc-std-w32-spht-1", sep="\t")
################# 90% large high cont #############################

################# 50% ro low cont ##################################
h2_1=pd.read_csv("runs/tpcc-std-w32-p8tm-si-v2-1", sep="\t")
h2_2=pd.read_csv("runs/x10-tpcc-std-w32-p8tm-psi-v2-1", sep="\t")
h2_3=pd.read_csv("runs/x10-tpcc-std-w32-p8tm-psi-v2-fi-1", sep="\t")
h2_4=pd.read_csv("runs/tpcc-std-w32-htm-sgl-2", sep="\t")
h2_5=pd.read_csv("runs/x10-tpcc-std-w32-spht-1", sep="\t")
################# 50% ro high cont #################################

################# 10% ro low cont ###############################
h3_1=pd.read_csv("runs/tpcc-std-w32-p8tm-si-v2-1", sep="\t")
h3_2=pd.read_csv("runs/x100-tpcc-std-w32-p8tm-psi-v2-1", sep="\t")
h3_3=pd.read_csv("runs/x100-tpcc-std-w32-p8tm-psi-v2-fi-1", sep="\t")
h3_4=pd.read_csv("runs/tpcc-std-w32-htm-sgl-2", sep="\t")
h3_5=pd.read_csv("runs/x100-tpcc-std-w32-spht-1", sep="\t")
################# 90% small high cont ##############################

####################################################################
threads= h1_1['threads']

h1=[h1_1, h1_2, h1_3, h1_4, h1_5]
h2=[h2_1, h2_2, h2_3, h2_4, h2_5]
h3=[h3_1, h3_2, h3_3, h3_4, h3_5]
h=[h1,h2,h3]

tx_h1=[0, 0, 0, 0, 0]
tx_h2=[0, 0, 0, 0, 0]
tx_h3=[0, 0, 0, 0, 0]

std_h1=[0, 0, 0, 0, 0]
std_h2=[0, 0, 0, 0, 0]
std_h3=[0, 0, 0, 0, 0]

for i in range(5):
    tx_h1[i]=h1[i]['Throughput_avg']/100000
    tx_h2[i]=h2[i]['Throughput_avg']/100000
    tx_h3[i]=h3[i]['Throughput_avg']/100000
    std_h1[i]=h1[i]['Throughput_std']/100000
    std_h2[i]=h2[i]['Throughput_std']/100000
    std_h3[i]=h3[i]['Throughput_std']/100000


print(h1_1['rot_trans_avg'][2])



tx_h=[tx_h1, tx_h2, tx_h3]
std_h=[std_h1, std_h2, std_h3]

#print(h2_1['rot_trans_avg'])

fig1, ax1 = plt.subplots(nrows=1,ncols=4)
fig2, ax2 = plt.subplots(nrows=2,ncols=3)
fig3, ax3 = plt.subplots(nrows=2,ncols=3)
title=["SI-HTM","PSI","PSI flush\ninside sus/res","HTM","SPHT"]
title2=["P8 TPC-C high update txs latency","P8 TPC-C high update txs latency","P8 TPC-C high update txs latency"]
ax=[ax1,ax2,ax3]

x=('1t','2t','4t','8t','16t','32t')
x1=('1','10','100')


for j in range(3):
    for i in range(5):
        if i!=3:
            ax1[j+1].bar(title[i],h[j][i]['rot_nontrans_avg'][2],label="Non-Transactional",color="C"+str(0))
            ax1[j+1].bar(title[i],h[j][i]['rot_trans_avg'][2],bottom=h[j][i]['rot_nontrans_avg'][2],label="Transactional",color="C"+str(1))
            ax1[j+1].bar(title[i],h[j][i]['rot_capacity_avg'][2],bottom=h[j][i]['rot_nontrans_avg'][2]+h[j][i]['rot_trans_avg'][2],label="capacity",color="C"+str(2))
            ax1[j+1].bar(title[i],h[j][i]['rot_user_avg'][2],bottom=h[j][i]['rot_nontrans_avg'][2]+h[j][i]['rot_trans_avg'][2]+h[j][i]['rot_capacity_avg'][2],label="user abort",color="C"+str(3))
            ax1[j+1].bar(title[i],h[j][i]['rot_self_avg'][2],bottom=h[j][i]['rot_nontrans_avg'][2]+h[j][i]['rot_trans_avg'][2]+h[j][i]['rot_capacity_avg'][2]+h[j][i]['rot_user_avg'][2],label="self-abort",color="C"+str(4))
            if i==0:
                ax1[j+1].legend(loc='upper left')#,bbox_to_anchor=(-0.58, 0.90))
            ax1[j+1].set_title(x1[j])
            ax1[j+1].set_xlabel("Threads")
            ax1[j+1].set_ylabel("Aborts (%)")
            #ax[j][j,i+1].legend(loc='upper left')
        else:
            ax1[j+1].bar(title[i],h[j][i]['htm_nontrans_avg'][2],label="Non-Transactional",color="C"+str(0))
            ax1[j+1].bar(title[i],h[j][i]['htm_trans_avg'][2],bottom=h[j][i]['htm_nontrans_avg'][2],label="Transactional",color="C"+str(1))
            ax1[j+1].bar(title[i],h[j][i]['htm_capacity_avg'][2],bottom=h[j][i]['htm_nontrans_avg']+h[j][i]['htm_trans_avg'][2],label="capacity",color="C"+str(2))
            ax1[j+1].bar(title[i],h[j][i]['htm_user_avg'][2],bottom=h[j][i]['htm_nontrans_avg']+h[j][i]['htm_trans_avg'][2]+h[j][i]['rot_capacity_avg'][2],label="user abort",color="C"+str(3))
            ax1[j+1].set_title(x1[j])
            #ax1[0,j+1].legend(loc='upper left')
            ax1[j+1].set_xlabel("Threads")
            ax1[j+1].set_ylabel("Aborts (%)")
            #ax[j][1,i].bar(x,h[j][i][2]['rot_self_avg'],bottom=h[j][i][2]['rot_nontrans_avg']+h[j][i][2]['rot_trans_avg']+h[j][i][2]['rot_capacity_avg'],label="self-abort")
            #ax[j][j,i+1].legend(loc='upper left')


##############################

#######################################
plots_si  =[tx_h[0][0][2],tx_h[1][0][2],tx_h[2][0][2]]
plots_psi =[tx_h[0][1][2],tx_h[1][1][2],tx_h[2][1][2]]
plots_psi2=[tx_h[0][2][2],tx_h[1][2][2],tx_h[2][2][2]]
plots_htm =[tx_h[0][3][2],tx_h[1][3][2],tx_h[2][3][2]]
plots_spht=[tx_h[0][4][2],tx_h[1][4][2],tx_h[2][4][2]]
all_plots=[plots_si,plots_psi,plots_psi2,plots_htm,plots_spht]
for j in range(3):
    for i in range(5):
        #ax[j][0,0].scatter(x,tx_h[j][i][2])
        #ax1[0,0].scatter(x1[j],tx_h[j][i][2][2], color="C"+str(i))
        ax1[0].plot(x1,all_plots[i], label=title[i],color="C"+str(i))
        #ax1[0,0].errorbar(x1[j],tx_h[j][i][2][2],  yerr=std_h[j][i][2][2], capsize=5, color="C"+str(i))
    ax1[0].set_title(title2[j])
    if j==0:
        ax[j][0].legend(loc='upper left',bbox_to_anchor=(-0.75, 0.90))
        ax[j][0].set_ylabel("Throughput (10$^5$)")
        ax[j][0].set_xlabel("Threads")
    else:
        ax[j][0,0].set_ylabel("Throughput (10$^5$)")
        ax[j][0,0].set_xlabel("Threads")




##fig.tight_layout()
#fig1.tight_layout()
##fig2.tight_layout()
##fig3.tight_layout()
##fig4.tight_layout()
##fig5.tight_layout()
##fig6.tight_layout()
##fig.subplots_adjust( left  = 0.125, right = 0.85, bottom = 0.1, top = 0.9, wspace = 0.25, hspace = 0.35)
fig1.subplots_adjust(left  = 0.125, right = 0.9, bottom = 0.288, top = 0.712, wspace = 0.25, hspace = 0.58)
fig2.subplots_adjust(left  = 0.125, right = 0.9, bottom = 0.1, top = 0.9, wspace = 0.25, hspace = 0.58)
fig3.subplots_adjust(left  = 0.125, right = 0.9, bottom = 0.1, top = 0.9, wspace = 0.25, hspace = 0.58)
#fig4.subplots_adjust(left  = 0.125, right = 0.9, bottom = 0.1, top = 0.9, wspace = 0.25, hspace = 0.58)
#fig5.subplots_adjust(left  = 0.125, right = 0.9, bottom = 0.1, top = 0.9, wspace = 0.25, hspace = 0.58)
#fig6.subplots_adjust(left  = 0.125, right = 0.9, bottom = 0.1, top = 0.9, wspace = 0.25, hspace = 0.58)
#
#ax[j][1,2].remove()
#ax2[1,2].remove()
#ax3[1,2].remove()
#ax4[1,2].remove()
#ax5[1,2].remove()
#ax6[1,2].remove()
#
#
plt.show()