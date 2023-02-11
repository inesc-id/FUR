from matplotlib import pyplot as plt
import pandas as pd

################# 90% large  low cont #############################
h1_1=pd.read_csv("runs/hashmap-hirocap-lowcon-u10-p8tm-si-v2-1", sep="\t")
h1_2=pd.read_csv("runs/hashmap-hirocap-lowcon-u10-p8tm-psi-v2-1", sep="\t")
h1_3=pd.read_csv("runs/hashmap-hirocap-lowcon-u10-p8tm-psi-v2-fi-1", sep="\t")
h1_4=pd.read_csv("runs/hashmap-hirocap-lowcon-u10-htm-sgl-2", sep="\t")
h1_5=pd.read_csv("runs/hashmap-hirocap-lowcon-u10-spht-1", sep="\t")
################# 90% large high cont #############################

################# 50% ro low cont ##################################
h2_1=pd.read_csv("runs/hashmap-hirocap-lowcon-u50-p8tm-si-v2-1", sep="\t")
h2_2=pd.read_csv("runs/hashmap-hirocap-lowcon-u50-p8tm-psi-v2-1", sep="\t")
h2_3=pd.read_csv("runs/hashmap-hirocap-lowcon-u50-p8tm-psi-v2-fi-1", sep="\t")
h2_4=pd.read_csv("runs/hashmap-hirocap-lowcon-u50-htm-sgl-2", sep="\t")
h2_5=pd.read_csv("runs/hashmap-hirocap-lowcon-u50-spht-1", sep="\t")
################# 50% ro high cont #################################

################# 10% ro low cont ###############################
h3_1=pd.read_csv("runs/hashmap-hirocap-lowcon-u90-p8tm-si-v2-1", sep="\t")
h3_2=pd.read_csv("runs/hashmap-hirocap-lowcon-u90-p8tm-psi-v2-1", sep="\t")
h3_3=pd.read_csv("runs/hashmap-hirocap-lowcon-u90-p8tm-psi-v2-fi-1", sep="\t")
h3_4=pd.read_csv("runs/hashmap-hirocap-lowcon-u90-htm-sgl-2", sep="\t")
h3_5=pd.read_csv("runs/hashmap-hirocap-lowcon-u90-spht-1", sep="\t")
################# 90% small high cont ##############################

####################################################################
threads= h1_1['threads']
print(h1_1['threads'])

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


tx_h=[tx_h1, tx_h2, tx_h3]
std_h=[std_h1, std_h2, std_h3]

#print(h2_1['rot_trans_avg'])

fig1, ax1 = plt.subplots(nrows=2,ncols=3)
fig2, ax2 = plt.subplots(nrows=2,ncols=3)
fig3, ax3 = plt.subplots(nrows=2,ncols=3)
title=["SI-HTM","PSI","PSI flush\ninside sus/res","HTM","SPHT"]
title2=["P8 Hash-map 90% read-only txs 200k initial items","P8 Hash-map 50% read-only txs 200k initial items","P8 Hash-map 10% read-only txs 200k initial items"]
ax=[ax1,ax2,ax3]

x=('1t','2t','4t','8t','16t','32t')

for j in range(3):
    for i in range(5):
        d=0
        y=i+1
        if i>=2:
            d=1
            y=y-3
        if i!=3:
            ax[j][d,y].bar(x,h[j][i]['rot_nontrans_avg'],label="Non-Transactional")
            ax[j][d,y].bar(x,h[j][i]['rot_trans_avg'],bottom=h[j][i]['rot_nontrans_avg'],label="Transactional")
            ax[j][d,y].bar(x,h[j][i]['rot_capacity_avg'],bottom=h[j][i]['rot_nontrans_avg']+h[j][i]['rot_trans_avg'],label="capacity")
            ax[j][d,y].bar(x,h[j][i]['rot_user_avg'],bottom=h[j][i]['rot_nontrans_avg']+h[j][i]['rot_trans_avg']+h[j][i]['rot_capacity_avg'],label="user abort")
            ax[j][d,y].bar(x,h[j][i]['rot_self_avg'],bottom=h[j][i]['rot_nontrans_avg']+h[j][i]['rot_trans_avg']+h[j][i]['rot_capacity_avg']+h[j][i]['rot_user_avg'],label="self-abort")
            ax[j][1,0].legend(loc='upper left',bbox_to_anchor=(-0.58, 0.90))
            ax[j][d,y].set_title(title[i])
            ax[j][d,y].set_xlabel("Threads")
            ax[j][d,y].set_ylabel("Aborts (%)")
            #ax[j][j,i+1].legend(loc='upper left')
        else:
            ax[j][d,y].bar(x,h[j][i]['htm_nontrans_avg'],label="Non-Transactional")
            ax[j][d,y].bar(x,h[j][i]['htm_trans_avg'],bottom=h[j][i]['htm_nontrans_avg'],label="Transactional")
            ax[j][d,y].bar(x,h[j][i]['htm_capacity_avg'],bottom=h[j][i]['htm_nontrans_avg']+h[j][i]['htm_trans_avg'],label="capacity")
            ax[j][d,y].bar(x,h[j][i]['htm_user_avg'],bottom=h[j][i]['htm_nontrans_avg']+h[j][i]['htm_trans_avg']+h[j][i]['rot_capacity_avg'],label="user abort")
            ax[j][d,y].set_title(title[i])
            ax[j][d,y].set_xlabel("Threads")
            ax[j][d,y].set_ylabel("Aborts (%)")
            #ax[j][1,i].bar(x,h[j][i]['rot_self_avg'],bottom=h[j][i]['rot_nontrans_avg']+h[j][i]['rot_trans_avg']+h[j][i]['rot_capacity_avg'],label="self-abort")
            #ax[j][j,i+1].legend(loc='upper left')


##############################

#######################################

for j in range(3):
    for i in range(5):
        #ax[j][0,0].scatter(x,tx_h[j][i])
        ax[j][0,0].errorbar(x,tx_h[j][i],  yerr=std_h[j][i], capsize=5, color="C"+str(i))
        ax[j][0,0].plot(x,tx_h[j][i], label=title[i],color="C"+str(i))
    ax[j][0,0].set_title(title2[j])
    ax[j][0,0].legend(loc='upper left',bbox_to_anchor=(-0.55, 0.90))
    ax[j][0,0].set_ylabel("Throughput (10$^5$)")
    ax[j][0,0].set_xlabel("Threads")




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
#ax[j][1,2].remove()
#ax2[1,2].remove()
#ax3[1,2].remove()
#ax4[1,2].remove()
#ax5[1,2].remove()
#ax6[1,2].remove()
#
#
plt.show()