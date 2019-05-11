title: Python re库 正则表达式
date: 2015-10-22 17:38:18
tags: [python, 转载]
categories: 编程
---
以下所有例子都有import re这一句，妇孺皆知。


### 查找文本中的字符
<!-- more -->

    pattern = 'this'
    text = 'Does this text match the pattern?'
    match = re.search(pattern,text)
    s = match.start()
    e = match.end() 
    print 'Found "%s"\nin "%s"\nfrom %d to %d ("%s")' %\
          (match.re.pattern,match.string,s,e,text[s:e])
    '''
    match.re.pattern 要匹配的内容
    match.string 匹配的字符
    s  匹配到内容开始索引
    d  匹配到内容结束索引
    text[s:e] 匹配字符
    '''
 
### 编译表达式

使用re.compile()函数，将正则表达式的字符串形式编译为Pattern实例（Regexobject），然后使用Pattern实例处理文本并获得匹配结果（一个Match实例），最后使用Match实例获得信息，进行其他的操作。

re.compile(pattern[, flags])
把一个正则表达式pattern编译成正则对象，以便可以用正则对象的match和search方法。
得到的正则对象的行为（也就是模式）可以用flags来指定，值可以由几个下面的值OR得到。

以下result1,result2在语法上是等效的：

    prog = re.compile(pattern)
    result1 = prog.match(string)
    result2 = re.match(pattern, string)

区别是，用了re.compile以后，正则对象会得到保留，这样在需要多次运用这个正则对象的时候，效率会有较大的提升。再用上面用过的例子来演示一下，用相同的正则匹配相同的字符串，执行100万次，就体现出compile的效率了


寻找一个字符串中所有的英文字符：

    pattern = re.compile('[a-zA-Z]')
    result = pattern.findall('as3SiOPdj#@23awe')
    print result
	# ['a', 's', 'S', 'i', 'O', 'P', 'd', 'j', 'a', 'w', 'e']
 
另一个例子：

    regexes = [ re.compile(p)
                for p in ['this','that']              
    ] #把字符转换Regexobject格式
    print 'Text: %r\n' % text #输出text内容
    for regex in regexes:
        print 'Seeking "%s"->' % regex.pattern,  #regex.pattern 要匹配的字符
        if regex.search(text): #在text中搜索this or that
            print 'match!'
        else:
            print 'no match'
 
### 多重匹配findall
 
    text = 'abbaaabbbbaaaaa'
    pattern = 'ab'
    for match in re.findall(pattern,text):
        print 'Found: "%s"' % match
    #findall 直接返回字符串
    for match in re.finditer(pattern,text):
        s = match.start()
        e = match.end()
        print 'Found "%s" at %d:%d' % (text[s:e],s,e)
    #finditer 返回原输入文字在字符串的位置
 
### 模式语法
 
	def test_patterns(text,patterns=[]):
    for pattern,desc in patterns: 
        print 'Pattern %r (%s) \n' %(pattern,desc) 
        print '   %r' % text
        for match in re.finditer(pattern,text):
            s = match.start()
            e = match.end()
            substr = text[s:e] #匹配到的字符
            n_backslashes = text[:s].count('\\') #查找文本:s坐标之前的包含多少\\
            prefix = '.' * ( s + n_backslashes ) 
            print '    %s%r' % (prefix,substr) 
        print
    return
 
    test_patterns('abbaaabbbbaaaaa',
                [('ab',"'a' followed by 'b'")]
        )
    #贪婪模式 这种模式会减少单个匹配减少
    '''
         ab*,      'a followerd by zero or more b'),  #匹配0次或者更多次
         ab+,      'a followerd by one or mrore b'),  #最少匹配一次或者更多次
    '''
    #用法如下:
    str = 'absdsdsdsdsd'
    print re.findall('ab*',str)
    #['ab']

    print re.findall('ab*?',str)
    #['a']
 
### 字符集
 
    '''
    [ab]     'either a or b 匹配a或者b'
    a[ab]+   'a followerd by 1 more a or b 匹配一次a、b或者多次 '
    a[ab]+?  'a followerd by 1 or more a or b,not greedy 匹配1一次可以匹配多次'
    [^]      '不包含内容'
    [a-z]    '所有小写ASCII字母' 
    [A-Z]    '所有大写写ASCII字母' 
    [a-zA-Z] '一个小写和大写的序列'
    [A-Za-z] '一个大写小写的序列'
    '''
    str ='aaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbabbbbbbbbbbbasbsbab,a_baba'
    print re.findall('[ab]',str)
    print re.findall('a[ab]+',str)
    print re.findall('a[ab]+?',str)
    print re.findall('[^_]',str)

    str = 'China,lovE'
    print re.findall('[a-z][A-Z]',str)  #['vE'] 
    print re.findall('[A-Z][a-z]',str)  #['Ch']
    print re.findall('[A-Z][a-z]+',str) #['China']
    print re.findall('[a-z][A-Z]+',str) #['vE']
    print re.findall('[A-Z][a-z]*',str) #['China', 'E']
    print re.findall('[a-z][A-Z]*',str) #['h', 'i', 'n', 'a', 'l', 'o', 'vE']
    print re.findall('[A-Z][a-z]?',str) #['Ch', 'E']
    print re.findall('[a-z][A-Z]?',str) #['h', 'i', 'n', 'a', 'l', 'o', 'vE']

    '''
    .      元字符匹配一个字符
    a.
    b.
    a.*b
    a.*?b
    '''
    c = 'woaizhongguoawsb,wasssssssssssssdsdsdsdbsdddddddbaaabbbbbbbsd'
    print re.findall('a.',c)  #['ai', 'aw', 'as', 'aa', 'ab']
    print re.findall('b.',c)  #['b,', 'bs', 'ba', 'bb', 'bb', 'bb', 'bs']
    print re.findall('a.*b',c)  #['aizhongguoawsb,wasssssssssssssdsdsdsdbsdddddddbaaabbbbbbb'] #贪婪模式匹配a到b之间的任意字符长度字符
    print re.findall('a.*?b',c)  #['aizhongguoawsb', 'asssssssssssssdsdsdsdb', 'aaab'] # ?结束了* 的贪婪模式,
                                 #它不会到最后一个b再去匹配而且见好就收,匹配可能最短的字符

### 转义码

    '''
    转义码                                   含义
     \d                                    一个数字
     \D                                    一个非字符
     \s                                    空白符(制表符、空格、换行符)
     \S                                    非空白符(符号、字母、数字)
     \w                                    字母数字
     \W                                    非字母数字(符号、制表符、空格、换行符)
    '''
 
### 锚定----定位
 
    '''
    锚定码                               含义
      ^                              字符串或行的开始
      $                              字符串或行结束
      \A                             字符串开始
      \Z                             字符串结束
      \b                             一个单词开头或者末尾的空串
      \B                             不在一个单词的开头活末尾的空串
    '''
### 限制搜索 match、search
 
    text = 'This is some text --with punctuation.'
    pattern = 'is'
    print 'Text    :',text
    print 'pattern:',pattern
    m = re.match(pattern,text)   #因为match是从字符开头开始匹配 is没有在开头所以没有匹配到.
    print 'Match :',m   
    s = re.search(pattern,text) #is在文本中出现了两次所以匹配到内容
    print 'Search :',s
    pattern = re.compile(r'\b\w*is\w*\b') #编译规则
    print 'Text:',text

    pos = 0
    while  True:
        match = pattern.search(text,pos) #搜索规则
        if not match:
            break
        s = match.start()
        e = match.end() 
        print '  %d : %d = "%s"' % (s,e-1,text[s:e]) 
        pos = e

### 用户组解析匹配----嵌套

	#任何一个正则都可以为组并嵌套在一个更大的表达式中
    regex = re.compile(r'(\bt\w+)\W+(\w+)')
    print 'Input  text      :',text
    print 'Pattern          :',regex.pattern
    match = regex.search(text)
    print 'Entire match     :',match.group(0) #表示整个表达式的字符串,子组从1开始排序
    print 'World start with "t":',match.group(1) #匹配到的第一组
    print 'World after "t" word :',match.group(2) #匹配到的第二组
    #python对基本分组进行了扩展 (?P<name>pattern)
    print text
    print
    for pattern in [ r'^(?P<first_word>\w+)',  #组名和正则表达式组成
                     r'(?P<last_word>\w+)\S*$',
                     r'(?P<t_word>\bt\w+)\W+(?P<other_word>\w+)',
                     r'(?P<ends_with_t>\w+t)\b',
                     ]:
        regex = re.compile(pattern)
        match = regex.search(text)
        print 'Matching "%s"' % pattern
        print ' ',match.groups()  #匹配到所有的组的值
        print ' ',match.groupdict() #把组名和字串生成字典 
        print

    def test_patterns(text,patterns=[]):
        '''Given source text and a list of patterns,look for 
        matches for each pattern within the text and print
        them to stdout.
        '''
        #look for each pattern in the text and print the resuls
        for pattern,desc in patterns:
            print 'Pattern %r (%s)\n' % (pattern,desc)
            print '   %r' % text
        for match in re.finditer(pattern,text):
            s = match.start()
            e = match.end()
            prefix = ' ' * (s) #'空格 X 次数'
            print '   %s%r%s' % (prefix,text[s:e],' '*(len(text)-e)),
            print match.groups()
            if match.groupdict():
                print '%s%s' % (' ' * (len(text) -s),match,groupdict())
                print
        return

    print test_patterns(text,[(r'(a(a*)(b*))','a followerd by 0-n a and 0-n b')])

    '''
    |       代表左右表达式任意匹配一个,他总是先尝试匹配左边的表达式,一旦成功匹配则
    跳过匹配右边的表达式。如果|没有被包括()中,则它的范围是整个正则表达式
    ?:pattern
    '''


### 搜索忽略大小写

    '''
    re.IGNORECASE 忽略大小写
    '''
    text  = 'This is some text  -- with punctuation.'
    pattern = r'\bT\w+'
    with_case = re.compile(pattern)
    whitout_case = re.compile(pattern,re.IGNORECASE) #re.IGNORECASE 忽略大小写
    print 'Text: \n  %r' % text
    print 'Pattern:\n %s' % pattern
    print 'Case-sensitive:'
    for match in with_case.findall(text):
        print '  %r' % match
    print 'Case-insensitive:'
    for match in whitout_case.findall(text):
        print ' %r' % match
 
### 多行输入匹配

    '''
    MULTILINE  多行匹配
    '''

    text = 'This is some text  -- with punctuation.\nA secone lines.'
    pattern = r'(^\w+)|(\w+\S*$)'
    single_line = re.compile(pattern)
    multiline = re.compile(pattern,re.MULTILINE) 
    print 'Text:\n %r' % text
    print 'Pattern:\n  %s' % pattern
    print 'Single Line :'
    for match in single_line.findall(text):
        print '  %r' % (match,)
    print 'MULTILINE  :'
    for match in multiline.findall(text):
        print '  %r'  % (match,)

    '''
    DOTALL 让点字符也可以匹配换行符
    '''
    pattern = r'.+'
    no_newlines = re.compile(pattern)
    dotall = re.compile(pattern,re.DOTALL)

    print 'Text :\n   %r' % text
    print 'Pattern:\n %s' % pattern
    print 'No newlines :'
    for match in no_newlines.findall(text):
        print '  %r' % match
    print 'Dotall    :'
    for  match in dotall.findall(text):
        print '  %r' % match

### Unicode匹配

    '''
    re.UNICODE 匹配Unicode
    '''
    import codecs
    import sys

    #set standard output encoding to UTF-8
    sys.output = codecs.getwriter('UTF-8')(sys.stdout)
    pattern = ur'\w+'
    ascii_pattern = re.compile(pattern)
    unicde_pattern = re.compile(pattern,re.UNICODE)

    print 'Text    :',text
    print 'Pattern :',pattern
    print 'ASCII   :',u', '.join(ascii_pattern.findall(text))
    print 'Unicode :',u', '.join(unicde_pattern.findall(text))
    '''
    re.VERBOSE 让正则更容易读
    '''
    address = re.compile(
            '''
            [\w\d.+-]+    #username
            @ 
            ([\w\d.]+\.)+ #domain name prefix
            (com|org|edu) #TODO:support more top-level domains
            ''',
            re.UNICODE | re.VERBOSE)

    candidates = [
            u'first.last@example.com',
            u'first.last+category@gmail.com',
            u'valid-address@mail.example.com',
            u'not-valid@example.foo'
    ]

    for candidate in candidates:
        match = address.search(candidate)
        print '%-30s %s' % (candidate,'Matche' if match else 'no match')

    address = re.compile (
        '''
        #A name is made up of letters,and may include "."
        #for title abbreviations and middle initials.
        ((?P<name>
            ([\w.,]+\S+)*[\w.,]+)
            \s*
            # Email addresses are wrapped in angle
            # brackets: <> but only if a name is 
            # found, so keep the start bracket in this
            # group.
            <
        )?  # the entire name is optional

        # the address itself:username@domain.tld
        (?P<email>
            [\w\d.+-]+    #username
            @ 
            ([\w\d.]+\.)+ #domain name prefix
            (com|org|edu) #TODO:support more top-level domains
        )
        >? # optional closeing angle break
        ''',
        re.UNICODE | re.VERBOSE)

    candidates = [
            u'first.last@example.com',
            u'first.last+category@gmail.com',
            u'valid-address@mail.example.com',
            u'not-valid@example.foo'
            u'Fist Last <first.last@example.com>'
            u'NO Brackets first.last@example',
            u'First Last',
            u'First Middle Last <first.last@example.com>',
            u'First M. Last <first.last@example.com>',
            u'<first.last@example.com>',
    ]

    for candidate in candidates:
        print 'candidate:',candidate
        match = address.search(candidate)
        if match:
            print ' Name:',match.groupdict()['name']
            print ' Email:',match.groupdict()['email']
        else:
            print '   No match'

    '''
                        正则表达式标志缩写表

      标志                    缩写        描述
      IGNORECASE              i          忽略大小写
      MULTILINE               m          多行匹配
      DOTALL                  s          让点字符也可以匹配换行符
      UNICODE                 u          匹配Unicode
      VERBOSE                 x          让正则更容易读
    在模式中嵌入标签(?imu)会打开相应的选项
    '''
    text = 'This is  some text -- with punctuation.'
    pattern = r'(?i)\bT\w+'
    regex = re.compile(pattern)

    print 'Text   :',text
    print 'Pattern    :',pattern
    print 'Matches   :',regex.findall(text)

### 前向或后向
 
    address = re.compile(
        '''
        # A name is made up of letters, and may include "."
        # for title abbreviations and middle initials
        ((?P<name>
            ([\w.,]+\s+)*[\w.,]+
            )
        \s+
        )  # name is no longer optional
        # LOOKAHEAD
        # Email address are wrapped in angle brackets, but only
        # if they are both present or neither is .
        (?= (<.*>$)
            |
            ([^<].*[^>]$)
        )
        <? # optional opening angle bracket

        # The address itself: username@domain.tld
        (?P<email>
            [\w\d.+-]+
            @
            ([\w\d.]+\.)+
            (com|org|edu)
        )
        >?
        ''',
        re.UNICODE | re.VERBOSE)

    candidates = [
        u'First Last <first.last@example.com>',
        u'No Brackets first.last@example.com',
        u'Open Brackets <first.last@example.com>',
        u'Close Brackets first.last@example.com',
        ]
    for candidate in candidates:
        print 'Candidate:',candidate
        match = address.search(candidate)
        if match:
            print ' Name :',match.groupdict()['name']
            print ' Email :',match.groupdict()['email']
        else:
            print '  No match'

    #自动忽略系统常用的noreply邮件地址
    '''
    (?!noreply@.*$) 忽略这个邮件地址
    (?<!noreply>)  两种模式 写在username之前不会向后断言 
    (?<=pattern)   用肯定向后断言查找符合某个模式的文本 
    '''
    address = re.compile(
        '''
        ^
        # An address: username@domain.tld

        # Ignore noreply address
        (?!noreply@.*$)

        [\w\d.+-]+     # username
        @
        ([\w\d.]+\.)+  # domain name prefix
        (com|org|edu)  # limit the allowed top-level domains

        $
        ''',
        re.UNICODE | re.VERBOSE)

    candidates = [

        u'first.last@example.com',
        u'noreply@example.com',
    ]

    for candidate in candidates:
        print 'Candidate:',candidate
        match = address.search(candidate)
        if match:
            print '  Match:',candidate[match.start():match.end()]
        else:
            print '  No match'

    twitter = re.compile(
        '''
        # A twitter handle: @username
        (?<=@)
        ([\w\d_]+)   # username
        ''',
        re.UNICODE | re.VERBOSE)

    text = ''' This text includes two Twitter handles.
    One for @TheSF,and one for the author,@doughellmann.
    '''
    print text
    for match in twitter.findall(text):
        print 'handle:',match

### 自引用

	#可以把表达式编号后面来引用
    address = re.compile(
        '''
        (\w+)          # first name
        \s+
        (([\w.]+)\s+)?  # optional middle name or initial
        (\w+)           # last name

        \s+
        <

        # The address: first_name.last_name@domain.tld
        (?P<email>
            \1         #first name
            \.
            \4         #last name
            @
            ([\w\d.]+\.)+
            (com|org|edu)
            )            
        >
        ''',
        re.UNICODE | re.VERBOSE | re.IGNORECASE)

    candidates = [
        u'First Last <first.last@example.com>',
        u'Different Name <first.last.example.com>',
        u'First Middle Last <first.last@example.com>', 
    ]
    for candidate in candidates:
        print 'Candidate:',candidate
        match = address.search(candidate)
    if match:
        print '  Match name:',match.group(1),match.group(4)
    else:
        print ' No match'

    #正则表达式解析包括一个扩展,可以使用(?P=name)指示表达式先前匹配的一个命名组的值.

    address = re.compile(
        '''

        # The regular name
        (?P<first_name>\w+)
        \s+
        (([\w.]+)\s+)?
        (?P<last_name>\w+)
        \s+
        <

        # The address: first_name.last_name@domain.tld
        (?P<email>
            (?P=first_name)
            \.
            (?P=last_name)
            @
            ([\w\d.]+\.)+
            (com|org|edu)
            )
        >
        ''',
        re.UNICODE | re.VERBOSE | re.IGNORECASE)

    candidates = [
        u'First last <first.last@example.com>',
        u'Different Name <first.last@example.com>',
        u'First Middle last <first.last@example.com>',
        u'First M. Last<first.last@example.com>',
    ]

    for candidate in candidates:
        print 'Candidate:',candidate
        match = address.search(candidate)
        if match:
            print '  Match name:',match.groupdict()['first_name']
            print match.groupdict()['last_name']
            print '  Match email:',match.groupdict()['email']

        else:
            print 'No match'

###  替换字符串

    '''
    re支持使用正则表达式作为搜索机制来修改文本，而且可以替换可以引用正则表达式中的匹配组作为替换文本的一部分。
    '''
    bold = re.compile(r'\*{2}(.*?)\*{2}')
    text = 'Make this **bold**. This **too**.'
    print 'Text:',text
    print 'Bold:',bold.sub(r'<b>\1</b>',text)
    '''
    使用命名组来替换
    count 来限制替换次数
    sbun 工作原理和sub相似 subn同时返回修改后的字符串和完成的替换次数
    '''
    bold = re.compile(r'\*{2}(?P<bold_text>.*?)\*{2}',re.UNICODE,)
    print 'Text:',text
    print 'Bold:',bold.sub(r'<b>\g<bold_text></b>',text,count=1)

    ### 利用模式拆分
    '''
    str.split() 是分解字符串来完成解析的最常用方法之一,它只是支持字面值得作为分隔符
    '''
    text = '''Paragraph one
    one tuo lines.

    Paragraph two.

    Paragraph three.'''

    print 'With findall:'
    for num,para in enumerate(re.findall(r'.+?\n{2,}|$',
                                        text,
                                        flags = re.DOTALL)
                                ):
        print num,repr(para)
        print

    print 
    print 'With split:'
    for num,para in enumerate(re.split(r'\n{2,}',text)):
        print num,repr(para)
        print
        
        
### 正则表达式实例
#### 去除网页基本元素

来源：[抓取淘宝MM爬虫](http://cuiqingcai.com/1001.html)（这个作者的博客好好看，技术很过关！），将处理写成一个类。调用即可

    import re
    #处理页面标签类
    class Tool:
        #去除img标签,以1-7位空格为例,&nbsp;
        removeImg = re.compile('<img.*?>| {1,7}|&nbsp;')
        #删除超链接标签
        removeAddr = re.compile('<a.*?>|</a>')
        #把换行的标签换为\n
        replaceLine = re.compile('<tr>|<div>|</div>|</p>')
        #将表格制表<td>替换为\t
        replaceTD= re.compile('<td>')
        #将换行符或双换行符替换为\n
        replaceBR = re.compile('<br><br>|<br>')
        #将其余标签剔除
        removeExtraTag = re.compile('<.*?>')
        #将多行空行删除
        removeNoneLine = re.compile('\n+')
        def replace(self,x):
            x = re.sub(self.removeImg,"",x)
            x = re.sub(self.removeAddr,"",x)
            x = re.sub(self.replaceLine,"\n",x)
            x = re.sub(self.replaceTD,"\t",x)
            x = re.sub(self.replaceBR,"\n",x)
            x = re.sub(self.removeExtraTag,"",x)
            x = re.sub(self.removeNoneLine,"\n",x)
            #strip()将前后多余内容删除
            return x.strip()
        
### 其他：

#### Raw String Notation
Raw string notation (r"text") keeps regular expressions sane. Without it, every backslash ('\') in a regular expression would have to be prefixed with another one to escape it. For example, the two following lines of code are functionally identical:

    >>> re.match(r"\W(.)\1\W", " ff ")
    <_sre.SRE_Match object at ...>
    >>> re.match("\\W(.)\\1\\W", " ff ")
    <_sre.SRE_Match object at ...>
    
When one wants to match a literal backslash, it must be escaped in the regular expression. With raw string notation, this means r"\\". Without raw string notation, one must use "\\\\", making the following lines of code functionally identical:

    >>> re.match(r"\\", r"\\")
    <_sre.SRE_Match object at ...>
    >>> re.match("\\\\", r"\\")
    <_sre.SRE_Match object at ...>
    
### 鸣谢
本文为转载，原文地址：[点我直达](http://brotherxing.blog.51cto.com/3994225/1576216/)