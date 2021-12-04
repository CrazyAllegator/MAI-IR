import logging
import argparse
from datetime import datetime
from urllib.parse import urlparse, unquote
import os
import re
import traceback

import wikipediaapi
import colorama

def normalize(text):
    return re.sub(r'[/\\:*?"<>|]', " ", text)

def short_category_name(full_name):
    return full_name.split(':')[-1]

def make_category_path(category_names):
    return "/".join([ normalize(short_category_name(category_name)) for category_name in category_names ])
   
def download_doc(wiki,category_names,value):
    try:
       page = wiki.page(value.title)
       filename = os.path.basename(urlparse(unquote(page.fullurl)).path).replace('_',' ')           
       dir_ = make_category_path(category_names)
       with open(os.path.join(dir_,filename+'.txt'), "w+", encoding='utf8') as f:            	
         text = page.text
         f.write(text)
    except Exception as e:
       print(colorama.Fore.RED+traceback.format_exc()) 

def create_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--categories', nargs='+', default= ["Категория:Художники Норвегии XX века"])
    return parser.parse_args()

def main():  
    args = create_args()    

    wiki = wikipediaapi.Wikipedia(language='ru',extract_format=wikipediaapi.ExtractFormat.WIKI)
    
    for category_name in args.categories:
      category = wiki.page(category_name)
      category_dir_name =  make_category_path([category_name,])      
      if not os.path.exists(category_dir_name):
         os.mkdir(category_dir_name)
    
      t1 = datetime.now()
 
      queue = [[ category_name,value] for value in category.categorymembers.values()] 
      i= 0      
      while queue:
        *category_names, value = queue.pop(0)     
        if value.ns == wikipediaapi.Namespace.MAIN:
          i+=1
          print(colorama.Fore.BLUE+'#{0} {1}/{2}'.format(i,make_category_path(category_names),value.title))          
          download_doc(wiki,category_names,value) 
        elif value.ns == wikipediaapi.Namespace.CATEGORY:
          sub_category = value.title 	     
          category_names.append(sub_category)          
          queue += [ [*category_names,v] for v in value.categorymembers.values()]            
          sub_category_dir = make_category_path(category_names)
          if not os.path.exists(sub_category_dir):
             os.makedirs(sub_category_dir)
          
    print("Time left: %s" % str(datetime.now() - t1))
    
    
if __name__ == "__main__":  
    colorama.init()  
    try:  
        print(colorama.Fore.YELLOW+"Начало работы.")
        main()
        print(colorama.Fore.YELLOW+"Работа завершена.")
    except KeyboardInterrupt:  # Ctrl+C
        print(colorama.Fore.YELLOW+"Прервано пользователем.")
    except:
        print(colorama.Fore.RED+traceback.format_exc())  