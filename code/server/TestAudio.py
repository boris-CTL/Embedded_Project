# import IPython.display as ipd

# with open('testAudio.txt', 'rb') as file:
#     data = file.read()

# # audio = bytearray(data.encode())
# print(type(data))
# print(len(data))
# print(hex(data[0]))

# audio = list(data)

# print(type(audio))
# print(len(audio))
# print(hex(audio[0]))

# ipd.Audio(data)

# from pydub import AudioSegment
# from pydub.playback import play

# song = AudioSegment.from_wav("/Users/zhengzhisheng/Desktop/大二下/嵌入式系統/FinalProject/code/server/test.wav")
# play(song)

# import pyaudio  
# import wave  

# #define stream chunk   
# chunk = 1024  

# #open a wav format music  
# f = wave.open(r"/usr/share/sounds/alsa/Rear_Center.wav","rb")  
# #instantiate PyAudio  
# p = pyaudio.PyAudio()  
# #open stream  
# stream = p.open(format = p.get_format_from_width(f.getsampwidth()),  
#                 channels = f.getnchannels(),  
#                 rate = f.getframerate(),  
#                 output = True)  
# #read data  
# data = f.readframes(chunk)  

# #play stream  
# while data:  
#     stream.write(data)  
#     data = f.readframes(chunk)  

# #stop stream  
# stream.stop_stream()  
# stream.close()  

# #close PyAudio  
# p.terminate()  

# from playsound import playsound

# # playsound('test.wav')
# playsound('rawAudio.wav')

from playsound import playsound

with open('rawAudio.txt', 'r') as file:
    data = file.read()

mapping = {
    "0": 0,
    "1": 1,
    "2": 2,
    "3": 3,
    "4": 4,
    "5": 5,
    "6": 6,
    "7": 7,
    "8": 8,
    "9": 9,
    "a": 10,
    "b": 11,
    "c": 12,
    "d": 13,
    "e": 14,
    "f": 15
}

with open('rawAudio.wav', 'wb') as file:
    print(type(data))
    count = 0
    num = 0
    for i in data:
        # file.write(mapping[i])
        if count==0:
            num = mapping[i]
            count = 1
        else:
            num = num*16+mapping[i]
            file.write(bytes([num]))
            count = 0
    file.close()

playsound('rawAudio.wav')