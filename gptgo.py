from freeGPT import gpt3
import sys
#C:/Users/kaushik/AppData/Local/Microsoft/WindowsApps/python3.10.exe "c:/Users/kaushik/OneDrive/Desktop/c program project/gptgo.py" "who is modi" > outputgptgo.txt

prompt = sys.argv[1]
#prompt= "i am pre typing it "#input("enter question ")
try:
    resp = gpt3.Completion.create(prompt=prompt)
    text={str(resp['text'])}
except Exception as e:
    text="try again i cannot reach "
    #print(f" try again")

with open("outputgptgo.txt", "w") as f:
    f.write(str(text))
f.close()

