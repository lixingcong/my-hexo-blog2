    #!/usr/bin/env python
# -*- coding: utf-8 -*-
# Create on: 2016年1月20日 下午2:32:04 by Lixingcong
# 说明： 没有集成自动抢课


import requests
import re
import os
from cookielib import LWPCookieJar

URL = 'http://jwxt1.cumt.edu.cn'
REFER_URL = URL
CHECK_CODE_URL = 'http://jwxt1.cumt.edu.cn/CheckCode.aspx'
USERNAME = ''
PASSWORD = ''
URL_AFTER_LOGIN = 'http://jwxt1.cumt.edu.cn/xs_main.aspx?xh='
SELECT_POST_URL = 'http://jwxt1.cumt.edu.cn/xsxjs.aspx'
RE_IS_SUCCESS = re.compile('alert\(\'(.*)\'\);</script>', flags = 0)

course_list = []
course_len = 0
select_len = 0
select_list = []    # 0:名字 1:ID 2:上课时间
select_list_amount = []
VIEWSTATE = None
xkkh = None

# state
SHOW_COURSE_LIST = 0
SHOW_TEACHER_LIST = 1
FUCK_TIME = 2

def parse_course_list(course_page):
    global course_len
    global course_list
    RE_course_status = re.compile(u'[已未]选', flags = 0)
    RE_course_url = re.compile(u'<td><a\shref=\'#\'\sonclick=\"window\.open\(\'(.{,130})\',\'xsxjs')    
    RE_course_name = re.compile(u'scrollbars=1,resizable=1\'\)\">(.{,60})</a>', flags = 0)    
    for line in course_page:
        
        if len(line) < 500 or len(line) > 1000:
            continue
        LINE = line.decode('gb2312')
        course_status = RE_course_status.findall(LINE)[0]    # 最坑爹就是这里，要返回group(0)
        course_url = RE_course_url.findall(LINE)[1]
        course_name = RE_course_name.findall(LINE)[1]
        # print course_name
        if course_status:
            course_list.append([course_name, course_url, course_status])
            course_len += 1
    if course_len == 0:
        print "Empty course list"
        
def parse_select_list(select_page):
    global VIEWSTATE
    global select_len
    global select_list    # 0:名字 1:ID 2:上课时间
    global VIEWSTATE
    global xkkh
    global select_list_amount
    RE_course_url = re.compile(u'window\.open\(\'jsxx.aspx\?xh=' + USERNAME + u'&xkkh=(.*)&amp;jszgh.*href=\"#\"\s>(.*)</A></TD>')
    RE_course_time = re.compile(u'<TD>(周[一二三四五六日]第.*)</TD>', flags = 0)
    RE_VIEWSTATE = re.compile('value=\"(.*)\"\s/>', flags = 0)
    RE_XKKH = re.compile('xkkh=(.*)&amp;xh=', flags = 0)
    RE_COURSE_AMOUNT = re.compile('<TD>(\d+)</TD>', flags = 0)
    for line in select_page:
        LINE = line.decode('gb2312').lstrip()
        if LINE.startswith("<TD><A onclick=\"window.open"):
            url = RE_course_url.findall(LINE)
            if url:
                select_list.append(url[0][1])
                select_list.append(url[0][0])
                select_len += 1
        else:
            course_time = RE_course_time.findall(LINE)
            if course_time:
                select_list.append(course_time[0])
            amount = RE_COURSE_AMOUNT.findall(LINE)
            if amount:
                select_list_amount.append(amount[0])
        if None == VIEWSTATE:
            if LINE.startswith("<input type=\"hidden\" name=\"__VIEWSTATE\""):
                VIEWSTATE = RE_VIEWSTATE.findall(LINE)[0]
        if None == xkkh:
            if LINE.startswith("<form name=\"xsxjs_form\" method=\"post\""):
                xkkh = RE_XKKH.findall(LINE)[0]
# cookie setting
s = requests.Session()
s.cookies = LWPCookieJar('cookiejar')
header = {'User-Agent':'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:38.0) Gecko/20100101 Firefox/38.0',
          'Referer':REFER_URL
          }

payload = {
       '__VIEWSTATE':'dDwyODE2NTM0OTg7Oz6gIzHUHUo9I4p23LsZjjNrYu9EnQ==',
       '__VIEWSTATEGENERATOR':'92719903',
       'txtUserName':None,
       'TextBox2':None,
       'txtSecretCode':None,
       'RadioButtonList1':'学生',
       'Button1':'',
       'lbLanguage':'',
       'hidPdrs':'',
       'hidsc':''
       }
payload_select = {
             '__EVENTTARGET':'Button1',
             '__EVENTARGUMENT':'',
             '__VIEWSTATE':None,
             '__VIEWSTATEGENERATOR':'AC27D4D4',
             'xkkh':None,
             'RadioButtonList1':'0'   
                }

# if cookies existed, or not expried
if os.path.exists('cookiejar'):
    ask = raw_input('You seem to login before. Autologin again? (y/n): ')
else:
    ask = 'n'

# login to the panel
if ask == 'n':
    USERNAME = raw_input("input username:")
    PASSWORD = raw_input("password:")
    payload['txtUserName'] = USERNAME
    payload['TextBox2'] = PASSWORD
    print "Downloading check code image..."
    check_code_page = s.get(CHECK_CODE_URL)
    try:
        os.remove('cookiejar')
    except :
        pass
    s.cookies.save(ignore_discard = True)
    with open('check.gif', 'wb') as code:
        code.write(check_code_page.content)
    # input
    print "Please open the file [check.gif] on this directory and fill in the blank."
    check_code = raw_input("input check code:")
    payload['txtSecretCode'] = check_code
    homepage = s.post(URL, data = payload, headers = header)
else:
    print "Reading cookies..."
    s.cookies.load(ignore_discard = True)
    homepage = s.get(URL_AFTER_LOGIN + USERNAME, headers = header)
   
# Welcome
try:
    re_name = re.compile(u'\sid="xhxm">(.*)同学</span></em>', flags = 0)
    my_name = re_name.findall(homepage.text)[0]
    print my_name, "，你好。\n"
except:
    print "Error!"
    try:
        os.remove('cookiejar')
    except:
        pass
    finally:
        exit(1)

state = SHOW_COURSE_LIST
is_list_fresh = False
while(True):
    if state == SHOW_COURSE_LIST:
        if not is_list_fresh:
            # Open the course list
            list_course_page = s.get('http://jwxt1.cumt.edu.cn/xsxk.aspx?xh=' + USERNAME + '&xm=' + my_name, headers = header)
            with open('list_page.txt', 'wb') as file1:
                file1.write(list_course_page.text.encode('gb2312'))
            with open('list_page.txt', 'r') as file1:
                parse_course_list(file1)
            is_list_fresh=True
        
        # print the list of courses
        temp = 0
        print '*' * 30
        for i in course_list:
            print "%d\t%s\t\t%s" % (temp, i[0], i[2])
            temp += 1
        choose_number = raw_input("if you want to quit, input e.\nplease choose a course:")
        if choose_number == 'e':
            exit(0)
        state = SHOW_TEACHER_LIST
        
    if state == SHOW_TEACHER_LIST:
        # Open a course
        select_course_page = s.get(URL + '/' + course_list[int(choose_number)][1])
        with open('select_page.txt', 'wb') as file1:
            file1.write(select_course_page.text.encode('gb2312'))
        with open('select_page.txt', 'r') as file1:
            parse_select_list(file1)
            payload_select['VIEWSTATE'] = VIEWSTATE

        # print a course
        temp = 0
        while temp < select_len:
            print "*"*30
            print "%d Teacher: %s\n\tAvaliabe/Total: %d/%d" % (temp, select_list[temp * 3],
                                                             int(select_list_amount[2 * temp]) - int(select_list_amount[2 * temp + 1]),
                                                             int(select_list_amount[2 * temp]))
            time_format = select_list[3 * temp + 2].split(';')
            for i in time_format:
                print "\t%s" % i
            temp += 1
    
        print '*' * 30
        choose_number = raw_input("input 'b' to back to the list, input e to quit\nwhich teacher do you prefer?")
        if choose_number == 'b':
            state = SHOW_COURSE_LIST
        elif choose_number == 'e':
            exit(0)
        else:
            state = FUCK_TIME

    if state == FUCK_TIME:
        payload_select['xkkh'] = select_list[int(choose_number) * 3 + 1]
        final = s.post(url = SELECT_POST_URL + '/?xkkh=' + xkkh + '&xh=' + USERNAME, data = payload_select, headers = header)
        with open('final.txt', 'wb') as f:
            f.write(final.text.encode('gb2312'))
        with open('final.txt', 'rb') as f:
            for line in f:
                LINE = line.decode('gb2312')
                alert1 = RE_IS_SUCCESS.findall(LINE)
                if alert1:
                    print alert1[0] + u"请注意下面页面是否成功选课！"
                    is_list_fresh = False
                    course_len = 0
                    course_list = []
                    select_len = 0
                    select_list = []
                    select_list_amount = []
        state = SHOW_COURSE_LIST
# print "%r" % ('http://jwxt1.cumt.edu.cn/xsxk.aspx?xh=04131434&xm=' + my_name)
