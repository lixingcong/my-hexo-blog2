title: Anki理解笔记
date: 2016-11-15 21:04:13
tags: [翻译, Anki]
categories: 读书笔记
---
Anki是我从2013年开始使用的卡牌软件，用于学习一门外语，或者突击复习材料繁多的考试。Anki自定义卡牌非常强大。近期花时间阅读了官方英文文档，在这里写下自己的翻译和理解。
<!-- more -->
## 简介

在背英语单词时，相信有人使用过一种“纸卡片”的方法：找一叠扑克牌大小的卡片，正反两面写上英文和中文意思，然后不断翻阅卡片，反复记忆，通常能达到比“机械地记忆”更好的效果。

Anki就是一个类似于纸卡片工作原理的软件，可以制作几乎所有类型的卡片，方便随时随地复习。

> Anki is a program which makes remembering things easy. Because it is a lot more efficient than traditional study methods, you can either greatly decrease your time spent studying, or greatly increase the amount you learn.

Anki应用领域非常广
- 语言学习：Arabic(阿拉伯语)，Chinese(汉语)，English(英语)...
- 备考：高考，USMLE(美国执业医师执照考试)，司法考试...
- 科学：解剖，病理学，化学，地理，音乐...
- 琐事：练习吉他和弦，温习唐诗宋词，生活记事本...

Anki主要特点

- 免费、开源，基于python+Qt
- 使用流行的SuperMemo算法记忆曲线
- 自定义卡片的CSS，支持Javascript，支持音频图片导入，LeTex公式
- 跨平台，支持windows/linux/macOS/Android/iOS(收费)
- 支持同步到AnkiWeb云端

Anki凭借自己独特的应用方式和使用思路已经完爆Anki的师傅/父亲：SuperMemo！！全球各地的前辈们自己使用着的卡片已经分享到了[AnkiWeb](https://ankiweb.net/shared/decks)了！！

## 术语

Anki中基本名词定义

|英文|中文|备注|
|--|--|--|
|Card|卡片|带有正反两面（含有问题和答案）的卡片，即"a question and answer pair"|
|Deck|卡牌集合（卡片组）|一个集合，里面装有很多卡片，可以导出为apkg文件|
|Note|笔记|生成卡片的一条数据库记录，一个笔记可以生成一张或者多张卡片|
|Field|字段|一个笔记的字段，类似于数据库的字段，常用的字段："FrontContent","BackContent","ImageFile"|
|Note Type|笔记类型|利用笔记生成卡片所用到的模板(Template)，类似于PowerPoint的母版|

其它

|英文|中文|备注|
|--|--|--|
|interval|复习间隔|概念不是学习步骤(learning step)中的"时间"，interval是卡片毕业后，该卡片下次出现的时间间隔|
|collection|收藏集|Anki中所有的资料，包括笔记、卡片、牌组、卡片类型|

## 添加卡片组

### 获取卡片组

可以下载(pre-made)别人的卡片组合，也可以自己创建(self-made)卡片组合。

|Decks|Pros|Crons|
|--|--|--|
|pre-made|方便快捷，来源广泛|词库庞大，不一定适合每一个人|
|self-made|自定义，符合个人记忆思路，效果更好|制作卡片花费时间较长|

> 'Self-made' vs 'pre-made'
> 
> Creating your own deck is the most effective way to learn a complex subject. Subjects like languages and the sciences can’t be understood simply by memorizing facts — they require explanation and context to learn effectively. Furthermore, inputting the information yourself forces you to decide what the key points are, leading to a better understanding.
>> "Do not learn if you do not understand." -- SuperMemo

总之，要“基于理解地记忆”，绝对不能是机械的记忆！对于记忆方法，强烈推荐看一下这篇文章[Effective learning: Twenty rules of formulating knowledge][20rules]，我花了不少时间理解其中的精髓！

如果您正在学习某本课本，而刚好有人分享了该课本的卡片组，这可以让你节省一些制作卡片的时间。不提倡直接拿别人的卡片，因为把别人的卡片组打开后会发现卡片很难懂，因为缺少相关背景资料和介绍。

## 学习

Anki采用"Learning Steps"概念进行卡片的学习，每点击Show Answer按钮，出现几个"Again","Good","Easy"按钮进行"Reset Step","Next Step","Graduate"或者步骤。

![](/images/anki/anki_steps.png)

每出现一次卡片，称为一个Learning Step，默认情况下只有；2个Step："1m, 10m"（意思Step1会卡片每1分钟出现一次。只要进入了step 2，那么大约10分钟后卡片再现。）

Learning Step是可以自定义的，比如我最近在学习日语记忆日文生词时，采用了这个自定义步骤（单位：分钟）：

	2 5 10 30 60
	
这里就有5个步骤，时间间隔如下

|Step|设置时间（分钟）|意义|
|--|--|--|
|1|2|初始步骤(点击Again会进入该步骤)，约2分钟后卡片再现|
|2|5|Step1->Step2，约5分钟后卡片将再次出现|
|3|10|Step2->Step3，约10分钟后卡片将再次出现|
|4|30|Step3->Step4，约30分钟后卡片将再次出现|
|5|60|Step4->Step5，约60分钟后卡片将再次出现|

如果在Step5记住了卡片（点击了"Good"或者"Easy"），那么卡片毕业了(Card is graduated)，卡片进入Review队列。Anki默认在下一天会再次出现（或者复习），随后将逐步增加出现的间隔（符合记忆曲线）。

建议每个人根据学习材料的难度，修改自定义步骤，不采用默认的"1 10"步骤

### Learning

点击一个Deck进入学习模式(Learning Mode)，对于新卡片才有“学习模式”这个概念。

学习模式下从左到右有三个打分按钮：Again,Good,Easy，桌面版的Anki，这三个按钮的快捷键是：1，2，3

下面分别介绍按钮的意义，对应的Learning Step处理方式。

#### Again

点击后将回到Step 1，没有任何副作用，生疏的卡片谁能一眼就记住呢？大家都是从不断的Again走过来的。

Anki旧版本这个按钮是"Soon"

![](/images/anki/anki_steps_again.png)

#### Good

点击后将进入Next Step，当处于Last Step时，卡片毕业(card is graduated)，卡片被放入待复习的队列。

![](/images/anki/anki_steps_good.png)

#### Easy

点击后将使得卡片毕业(card is graduated)，卡片被放入待复习的队列。

![](/images/anki/anki_steps_easy.png)

### Reviewing

对于已经学习的卡片（可以是已经毕业的卡片）复习。

从左到右有四个打分按钮：Again,Hard,Good,Easy。桌面版的Anki，这三个按钮的快捷键是：1，2，3，4

#### Again

标记该卡片为回答错误(incorrect)，并且要求Anki在未来尽可能地再现此卡片。

回答错误称为失误(card was lapsed)，关于失误后的默认处理是Anki将卡片的interval更改为1，即明天到期，并在接下来的10分钟内进入学习队列。

对失误(lapses)的处理，可以有更多的自定义：

	点击：Deck Option(卡片组选项) -> Lapses(失误)
	
1. New interval(新间隔)设定的是Anki对于回答失误的卡片的新复习时间间隔(interval)的百分值，基数是卡片的上一个时间间隔(last interval)。举个例子：一张卡片上一次的复习间隔是100天，New interval设置为20表示20%，那么失误后，新的interval被设定为20天。
2. Min interval(最小间隔)有效值值为不少于1的整数。默认值是1，表示Anki先通过New interval百分比计算得出的interval值，若该计算值大于Min interval，则被设定为Min interval值。
3. Leech threshold(难点阈值)指定连续"Again"失误的阈值，超过阈值将采取特定动作。
4. Leech action(难点动作)默认是到达记忆难点阈值后暂停卡片。

如果一张卡片进入了记忆难点（多次按Again达到难点阈值）被暂停，建议：
- Waiting，等到真正学习机会再学
- Deleting，删掉它，花时间学点简单的比死扣牛角尖更值得
- Editing，修改笔记，也许改一下Hint，加几个图片，或者重新排版一下CSS，让大脑接受新的学习环境再去攻克它

#### Hard

设定该卡片的新interval值为稍大于Good的interval值。效果是要求Anki在未来尽可能地再现此卡片（频率没有Again按钮高）。

#### Good

标记卡片的难度尚可，符合记忆曲线，告诉Anki不需要修正卡片的难度曲线(the card easiness doesn’t need to be adjusted down or up)。Anki默认将下次复习的间隔(interval)设定为250%乘以上一次的interval值。举例：上次看见卡片是10天前，今天看到后标记卡片Good，那么25天后才能再次看到这卡片。

#### Easy

标记卡片的难度变简单了，让Anki加长interval来让Anki在未来尽可能地不显示此卡片。通常对“确实特别简单的卡片”才这么做。一般建议点击Good即可。

### Overview数值

学习过程中有从左到右的三个带颜色数值：蓝色+棕色+绿色

	12 + 34 + 56
	
12代表是新卡片数量，34代表是正在学习的卡片数量，56代表待完成的复习卡片数（步骤越多，会随失败次数增加越多

### 卡片动作

- Mark Note(标记笔记)，将卡片所对应的笔记添加标签"Marked"，以便于在卡片浏览器中快速筛选。
- Bury Card(隐藏卡片)，将当前的笔记隐藏，下一天将自动取消隐藏。目的是留给明天学习（复习），至于手动取消隐藏，可以通过Deck Overview下的Unbury取消。隐藏卡片用于那些自己暂时不想学习的卡片。
- Suspend Car(暂停卡片)将当前的笔记暂停，直到手动取消暂停。常用于确实不想学习的卡片（或者特别难记的卡片，没学过的卡片...）

## 笔记类型

### 预置笔记

- Basic
最基本的卡片，只有两个字段：正面反面。

- Basic(Reverse)
可以从一个Note中生成两张卡片Card1,Card2，正反面能互相映射。

- Basic(Optional R.)
可控地从一个Note中生成两张卡片Card1,Card2。只需往Add Reverse字段填入任意字符就能生成Card2。如果Add Reverse为空则不生成Card2。

- Cloze
产生如下的填空题，用法详见知乎专栏[《cloze模板》][cloze_demo]

![](/images/anki/anki_cloze_demo.png)

### 自建笔记

因为最近自学日语，以建立日语生词卡片为例。

新建一个卡片类型

![](/images/anki/anki_note_type1.png)

add new from basic，名字“标准日语初级生词”。

![](/images/anki/anki_note_type2.png)

字段我自订为如下表

![](/images/anki/anki_note_type3.png)

|字段|意义|
|--|--|
|Chinese|中文意思|
|AudioFile|媒体文件|
|Japanese_1|平假名|
|Japanese_2|日本繁体字|
|Hint_ch2jp|自己写的提示，从中文联想出日文，或者写上自己的<br>理解，书中出现的页码数，也可以是例句等等|
|Part_of_speech|词性|

下一步就是自定义卡片的模板了

右上方有个加号，点一下可以添加一张新的卡片Card2，这样就可以利用一条Note记录产生正反对应的两张卡片，方便自己复习。

注意对Card1和Card2都要修改正反面的模板，不能两张卡都是单向映射对吧！要做到"中->日"和"日->中"的两张映射。

![](/images/anki/anki_note_type4.png)

- Anki支持HTML+CSS的布局渲染，加上Javascript(内嵌，不能外部引用)也是很好的
- 通过不同div class渲染不同的颜色
- 使用\{ \{hint:Hint_ch2jp\} \}能创建一个提示，当Hint字段不为空时候可以在学习卡片中显示Tips
- 使用Conditional Replacement可以生成不同的卡片，比如判断Part_of_speech为非空内容时候显示该词的词性

最后关闭卡片界面，到主菜单中添加卡片，卡片类型记得选中“标准日语初级生词”哦！

![](/images/anki/anki_note_type5.png)

开始学习了，可以设置自动播放音频

![](/images/anki/anki_note_type6.png)

通过设置卡片

## 卡片浏览器

浏览器的目的用于选中某些卡片进行修改等，或者调用正则表达式的批量替换。

### 侧栏

左侧显示了所有的Deck和Tag，卡片状态等

![](/images/anki/anki_browser1.png)

侧栏支持按键多选的逻辑：

- 按住Ctrl多选：产生逻辑AND
- 按住Shift多选：产生逻辑OR
- 按住Alt多选：产生逻辑 '-'，即排除

### 搜索语法

在查找栏输入特定的查找命令匹配collection里面的所有卡片

普通词如下

|查找命令|匹配|匹配举例|
|--|--|--|
|dog|含有dog|'doggy' 'underdog'|
|dog cat|一句话里面含有dog和cat|'a dog and a cat'|
|dog or cat|要么含有dog，要么含有cat|'doggy' 'catttt'|
|dog(cat or mouse)|dog和cat同时出现或者dog和mouse同时出现|-|
|-cat|不含cat|'mouse'|
|-cat -mouse|既不含猫也不含老鼠|'dog'|
|"a dog"|精确匹配a dog|'a dog'|
|-"a dog"|精确排除a dog|'a cat'|
|d_g|等价于正则表达式d[a-z]g|'dig' 'dzg'|
|d*g|等价于正则表达式d.?g|'d2g' 'dzzzg' 'dg'|

限定搜索字段：

|查找命令|匹配|
|--|--|
|front:dog|字段front含有dog|
|front:|字段front为空|
|front:_*|字段front非空|
|tag:animal|标签为animal|
|tag:ani*|标签开头为ani|
|tag:none|没有标签|
|deck:french -deck:french::*|属于french卡片组，但不属于french子卡片组|
|deck:filtered|已过滤卡片|
|card:1|卡片1|
|note:basic|卡片类型为basic|


限定卡片状态：

|查找命令|匹配|
|--|--|
|prop:ivl>=10|卡片的复习间隔>=10天|
|prop:due=1|明天到期的卡片|
|prop:due=-1|昨天到期的卡片，也就是没有及时复习的卡片|
|prop:due>-1 prop:due< 1|介于昨天和明天过期的卡片|
|prop:reps< 10|回答次数少于10次|
|prop:lapses>3|失误大于3|
|added:1|今天添加的卡片|
|added:7|上周添加的卡片|
|rated:1|今天回答过的卡片|
|rated:1:2|今天回答过的卡片中选过Hard(2)的卡片|
|rated:7:1|近7天回答过的卡片中选过Again(2)的卡片|

## 过滤卡片组

过滤卡片组(Filtered Deck)可以从某个Deck中抽取一部分卡片，用于特殊的学习目的
- 突击考试，不能全部复习，只能随机复习卡片
- 只复习特定的tag
- 累积已久老卡片复习
- 提前复习

已过滤的卡片不能使用已暂停(suspended)或者已隐藏(buried)的卡片。也不能使用已经被加入其它filtered deck的卡片。某个时刻一张卡片只能放在一个deck中。

删除filtered deck时候卡片被归回原来的卡片组(Home Deck)。

卡片选择排序(Filtered Deck Option选项下)：

|排序|说明|
|--|--|
|oldest seen first|按某卡片距离上次你看见它的时间排序，最长时间的优先|
|increasing interval|按复习间隔排序|
|ordered due|最近到期的优先|
|ordered added|添加顺序：从旧到新|
|ordered added first|添加顺序：从新到旧|
|relative overdueness|相关已过期，比如考虑如下2张卡片：卡片A的interval为5d，已经过期了2天还没复习；卡片B的interval为400d，已经过期7天，按照本排序算法，那么卡片A优先于B。这种排序方式通常用于筛选出最处于濒临忘记状态，但是只要抓住机会就有可能记起来的卡片|

## Excel导入

首先创建合适的笔记类型，还是以上面的“标准日语初级生词”卡片类型为例。

导入目的是含有所需笔记(Note)的TXT文档，若该文档满足卡片类型字段要求，Anki会自动映射字段并添加到特定Deck中。

字段
- Chinese
- AudioFile
- Japanese_1
- Japanese_2
- Hint_ch2jp
- Part_of_speech
- 标签

因为我电脑没有安装Office，故暂时使用LibreOffice代替表格处理软件。

![](/images/anki/anki_import1.png)

将该表格另存为TAB分割的txt纯文本，编码为utf-8。直接导入到Anki即可，注意指定合适的字段。

有时候由于词条重复，Anki忽略某些导入笔记。此时可以勾选”Ignore lines where first field matches exsiting note“即可强制导入到Deck。

![](/images/anki/anki_import2.png)

媒体文件(mp3,jpg)放入collection.media下，不应该有子目录（官方文档是这么说的）

打开Deck即可看到新导入的卡片

## 参考资料

[Anki设置自定义学习步骤：Software Review Redux: Anki][anki_steps]
[翻譯Anki用戶指南@大學生活紀事 - blogspot][anki_doc_taiwan]
[Effective learning: Twenty rules of formulating knowledge][20rules]
[知乎专栏:Anki-近乎完美的神器][zhihu_anki]

[anki_steps]: http://www.matcheducation.org/blog/2014/04/18/software-review-redux-anki
[cloze_demo]: https://zhuanlan.zhihu.com/p/21483899?refer=-anki
[anki_doc_taiwan]: http://wlhunag.blogspot.com/2013/06/Anki-tutorial-TOC.html
[20rules]: https://www.supermemo.com/en/articles/20rules
[zhihu_anki]: https://zhuanlan.zhihu.com/-anki
