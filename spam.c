#!/usr/bin/python
#-*-coding:utf-8-*-
import time
from urllib import urlencode
import cookielib, urllib2,urllib
import os,sys
import re
from xml.dom.minidom import parse, parseString
import getpass
from Queue import Queue
import threading

class Discuz:
	def __init__(self,uid,pwd,**param):
		self.username = uid
		self.password = pwd
		self.para = param
		self.regex = {
			'loginreg':'<input\s*type="hidden"\s*name="formhash"\s*value="([\w\W]+?)"\s*\/>',
			'postreg':'<input\s*type="hidden"\s*name="formhash"\s*id="formhash"\s*value="([\w\W]+?)"\s*\/>'
		}
		self.opener = None
		self.request = None
		self.islogin = False
		self.donecount = 0
		self.__login()
		self.threadcount = 100
		self.count = 0
		self.totalcount = 100000
	def __login(self):
		try:
			loginPage = urllib2.urlopen(self.para['loginurl']).read()
			formhash = re.search(self.regex['loginreg'],loginPage)
			formhash = formhash.group(1)
			print formhash
			print 'start login......'
			cookiejar = cookielib.CookieJar()
			self.opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(cookiejar))
			values = {
				'formhash':formhash,
				'username':self.username,
				'password':self.password,
				'loginsubmit':'true'
				}
			data = urllib.urlencode(values)
			self.request = urllib2.Request(self.para['loginsubmiturl'], data)
			rq = self.opener.open(self.request)
			print 'login success......'
			self.islogin = True
		except Exception ,e:
			print 'Loggin Error???:%s' % e
	def Post(self,subject,wysiwyg,content):
		threads = []


		for i in range(self.threadcount):

			t = threading.Thread(target=self.__postTopic,kwargs={'_subject':subject,'_wysiwyg':wysiwyg,'_body':content})
			threads.append(t)
		for i in range(self.threadcount):
			threads[i].start()
		lst = threading.enumerate()
		for i in range(self.threadcount):
			threads[i].join()
		print 'done'
	def __postTopic(self,**para):

		if not self.islogin:
			print 'please login......'
			return
		while self.count < self.totalcount:

			try:
				print 'current count %d:' % self.count
				print 'current thread name %s' % (threading.currentThread().getName())
				self.request = urllib2.Request(self.para['posturl'])
				rq = self.opener.open(self.request)
				data = rq.read()
				formhash = re.search(self.regex['postreg'],data)
				formhash = formhash.group(1)
				postdata = {
					'addtags':'',
					'checkbox':'0',
					'formhash':formhash,
					'iconid':'',
					'message':para['_body'],
					'subject':para['_subject'],
					'tags':'',
					'updateswfattach' : '0',
					'wysiwyg' : para['_wysiwyg']
				}
				self.request = urllib2.Request(self.para['postsubmiturl'],urllib.urlencode(postdata))
				self.opener.open(self.request)
				self.donecount+=1
				print '%d done.....!' % self.donecount
				#time.sleep(6)
				print 'OK'

			except Exception,e:
				print e
                #print "???WTF"
			if para.has_key('sleep'):
				print "OK"
				time.sleep(6)

			self.count +=1

if __name__=='__main__':

	name = 'YOUR_NAME'
	password = 'YOUR_PASSWORD'
	dz = Discuz(name,password,
	loginurl='THE_URL',
	loginsubmiturl='SUBMIT_URL',
	posturl='POST_URL',
	postsubmiturl='SUBMIT_URL',
	sleep='6'
	)
	content= 'CONTENT'


	dz.Post('POSTING','1',content)

