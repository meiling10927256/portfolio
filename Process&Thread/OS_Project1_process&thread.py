# -*- coding: utf-8 -*-

import time as t 
import multiprocessing as mp
import threading as td
import queue as Q
import datetime 

def BubbleSort( A ) :
	i = len( A ) - 1 
	while i > 0 :
		
		for j in range(i) :   
			if A[j] > A[j+1] :  # 如果前面比較大，就前後交換
				A[j], A[j+1] = A[j+1], A[j]
				
		i -= 1

	return A
	

def Merge( L, R, output ) :
	
	while len(L) > 0 and len(R) > 0 :
		if L[0] <= R[0] :
			output.append( L[0] )
			L.pop(0)
		else : # if L[0] > R[0] 
			output.append( R[0] )
			R.pop(0)

	if  len(L) > 0 :
		output.extend( L )
		# output.append( L[0] )
		#　L.pop(0)
		
	if  len(R) > 0 :
		output.extend( R )
		# output.append( R[0] )
		# R.pop(0)
	

def task2( A ) :
	i = 0
	while i < len(A) : 
		A[i] = BubbleSort(A[i])
		i += 1
	
	while len(A) > 1  : 
		output = []
		Merge( A[0], A[1], output ) 
		A.pop(0)
		A.pop(0)
		A.append(output)
		
def task3( A, i, part ) :
    if part == 1 : 
    	A[i] = BubbleSort( A[i] )
     
    if part == 2 :
        L = A.get()
        R = A.get()
        output = []
        Merge( L, R, output ) 
        A.put(output)
        # print(output) 

def task4( q ) :
	L = q.get()
	R = q.get()
	output = []
	Merge( L, R, output ) 
	# print(output)
	q.put(output)

	


if __name__ == "__main__" :

	# 讀檔
	name = input("請輸入檔案名稱:\n") 
	filename = name + '.txt'  # 輸入檔案名稱
	file = open( filename ,'r' )  # 開啟檔案
	
	num = []
	# 一次讀取一行文字，逐步讀取 
	for line in file.readlines() :
		num.append( int(line) )
		
	file.close() # 關閉檔案

	k = int(input("請輸入要切成幾份:\n"))
	way = int(input("請輸入方法編號:(方法1,方法2,方法3,方法4)\n"))
	
	runtime = 0

	if way == 1 :
		start = t.time()
		BubbleSort(num)
		end = t.time()
		runtime = end - start
		#　print( num )
	
	elif way == 2 : 
		divide = []
		p = len(num) // k # 商數
		r = len(num) % k # 餘數 
		for i in range(k) :
			divide.append(num[i*p:(i+1)*p])

		r = len(num) - r
		i = 0
		while r < len(num) :
			divide[i].append(num[r])
			i += 1
			r += 1

		# print(divide)

		# 父process與子process共享這個List
		chunk = mp.Manager().list(divide) 

		start = t.time()

		p1 = mp.Process( target = task2, args = ( chunk, ) ) 
		p1.start()
		p1.join()
	
		end = t.time()
		num = chunk[0]
		# print( end - start )
		# print( num )
		runtime = end - start
  
	elif way == 3 :
		divide = []
		p = len(num) // k # 商數
		r = len(num) % k # 餘數 
		for i in range(k) :
			divide.append(num[i*p:(i+1)*p])

		r = len(num) - r
		i = 0
		while r < len(num) :
			divide[i].append(num[r])
			i += 1
			r += 1

		# print(divide)
		# 父process與子process共享這個List
		chunk = mp.Manager().list(divide) 

		process_list = []

		start = t.time()
		# 開啟k個process來做bubblesort
		for i in range( k )  :
			process_list.append( mp.Process ( target = task3, args = ( chunk, i, 1 ) ) )  
			process_list[i].start()


		for i in range( k ) :
			process_list[i].join()
		
		# print(chunk)
		# print(len(chunk))
		# print(" finish p1 ")
		q = mp.Manager().Queue()
		for i in range(len(chunk)) :
			q.put( chunk[i] ) 
   
		process_list = []
		# 開啟k-1個process來做mergesort

		times = k // 2
		i = 0 
		m = 0
		while times > 0  :
			for j in range( times ) :
				process_list.append( mp.Process ( target = task3, args = ( q, i, 2 ) ) )  
				process_list[i].start()
				i += 1
    
			while m < i :
				process_list[m].join()
				m += 1
    
			# print("123")
			# print(times)
			# print( "q.size()" + str(q.qsize()) )
			if q.qsize() == 2 :
				times = 1
			else :
				times = q.qsize() // 2
 
			# print(times)
		# print(i)
		end = t.time()
		# print( end - start )
		runtime = end - start
		# print(" finish")
		num = q.get()

	elif way == 4 :
		divide = []
		p = len(num) // k # 商數
		r = len(num) % k # 餘數 
		for i in range(k) :
			divide.append(num[i*p:(i+1)*p])

		r = len(num) - r
		i = 0
		while r < len(num) :
			divide[i].append(num[r])
			i += 1
			r += 1

		# print(divide)
		# 父process與子process共享這個List
		# chunk = mp.Manager().list(divide) 

		start = t.time()

		thread_list = []
		# 開啟k個thread
		for i in range(k) :
			thread_list.append( td.Thread( target = BubbleSort, args = ( divide[i], ) ) )
			thread_list[i].start()
		
		# 等待所有子執行緒結束
		for i in range(k):
			thread_list[i].join()

		# print(chunk)

		thread_list = []
		# 開啟k-1個thread
		q = Q.Queue()
		for i in range(len(divide)) :
			q.put( divide[i] ) 

		times = k // 2
		i = 0 
		m = 0
		while times > 0  :
			for j in range( times ) :
				thread_list.append( td.Thread ( target = task4, args = ( q, ) ) )  
				thread_list[i].start()
				i += 1
    
			while m < i :
				thread_list[m].join()
				m += 1

			# print( "q.size()" + str(q.qsize()) )
			if q.qsize() == 2 :
				times = 1
			else :
				times = q.qsize() // 2

		end = t.time()
		# print( end - start )
		runtime = end - start
		# print(" finish")
		num = q.get()


	# print(runtime)
	# 寫檔
	filename = name + "_output" + str(way) + '.txt'
	file = open( filename, 'w+' )  # 建立一個新檔案，並寫入
	file.writelines( "Sort : \n" )

	for i in num :
		file.writelines(str(i))
		file.writelines('\n')    # 換行

	file.writelines("CPU Time :")
	file.writelines( str(runtime) )
	file.writelines('\n')
	file.writelines("Output Time :")
	file.writelines(str(datetime.datetime.now(tz=datetime.timezone(datetime.timedelta(hours=8)))))
	file.writelines('\n')

	file.close() # 關閉檔案