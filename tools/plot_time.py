import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import gridspec
from pathlib import Path

time_log_file = sys.argv[1]   

# 读取文本文件
remove_title = [] # 需要忽略的列
with open(time_log_file, 'r') as f:
    times_df = pd.read_csv(f)
    times_df = times_df[times_df.columns[:-1]]  # 移除最后一列
    titles = times_df.columns.values
    for title_index in remove_title:
        del times_df[titles[title_index]]
    times_df = times_df.dropna()
    titles = times_df.columns.values  # 读取标题

    # 计算统计量
    cell_text = []
    for item_id in titles:
        cell_text.append([round(times_df[item_id].mean(), 2), times_df[item_id].median(),
                          times_df[item_id].min(), times_df[item_id].max()])
        
# 画图
fig = plt.figure()
gs = gridspec.GridSpec(2, 1) 
# 画箱线图
plt.subplot(gs[0])
plt.grid(True)
times_df.boxplot(medianprops={'color': 'red'},  # 中值
                 meanline=True,
                 showmeans=True,
                 meanprops={'color': 'blue', 'ls': '--'},  # 均值
                 flierprops={"marker": "*",
                             "markerfacecolor": "red"},  # 异常值绘制
                 vert = False,   # 横着还是竖着
                 showfliers = False, # 是否显示异常值
                 # box属性
                 patch_artist=True,
                 boxprops={'facecolor': 'green', 'edgecolor': 'black'})

plt.subplot(gs[1])
# 添加统计数据表
cols = ['Mean', 'Median', 'Min', 'Max']
data_table = plt.table(cellText=cell_text,
                       rowLabels=titles,
                       colLabels=cols,
                       cellLoc='center',
                       rowLoc="center",
                       loc='center')
# data_table.set_fontsize(5)
# data_table.scale(1, 0.5)
plt.axis('off')

plt.savefig('time.png', bbox_inches='tight', dpi=1000)

# fig.tight_layout()
# plt.show()
