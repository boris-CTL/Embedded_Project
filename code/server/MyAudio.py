from playsound import playsound

def playWav(fileName):
    playsound(fileName+'.wav')

def txt2wav(data, fileName):
    mapping = {
        "0": 0, "1": 1, "2": 2, "3": 3, "4": 4, "5": 5, "6": 6, "7": 7, "8": 8, "9": 9, "a": 10, "b": 11, "c": 12, "d": 13, "e": 14, "f": 15
    }

    with open(fileName+'.wav', 'wb') as file:
        check = False
        num = 0
        for i in data:
            # file.write(mapping[i])
            if not(check):
                num = mapping[i]
                check = True
            else:
                num = num*16+mapping[i]
                file.write(bytes([num]))
                check = False
        file.close()

def txt2wav_double(data, fileName):
    mapping = {
        "0": 0, "1": 1, "2": 2, "3": 3, "4": 4, "5": 5, "6": 6, "7": 7, "8": 8, "9": 9, "a": 10, "b": 11, "c": 12, "d": 13, "e": 14, "f": 15
    }

    with open(fileName+'.wav', 'wb') as file:
        check = False
        num = 0
        count = 0
        for i in data:
            # file.write(mapping[i])
            if not(check):
                num = mapping[i]
                check = True
            else:
                num = num*16+mapping[i]
                file.write(bytes([num]))
                # file.write(bytes([num]))
                if count>=88:
                    file.write(bytes([num]))
                check = False
            count += 1
        file.close()

if __name__ == '__main__':
    with open('rawAudio.txt', 'r') as file:
        data = file.read()

    txt2wav_double(data, "rawAudio")
    playsound("rawAudio")