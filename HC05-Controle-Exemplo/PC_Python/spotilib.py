import win32gui
import win32api
import os
import spotipy
import pywinauto as appp
import time

from ctypes import cast, POINTER
from comtypes import CLSCTX_ALL
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume
devices = AudioUtilities.GetSpeakers()
interface = devices.Activate(
    IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
volume = cast(interface, POINTER(IAudioEndpointVolume))

spoti = appp.Application()
sp = spotipy.Spotify()

###Virtual-KeyCodes###
Media_Next = 0xB0
Media_Previous = 0xB1
Media_Pause = 0xB3 ##Play/Pause
Media_Mute = 0xAD
Media_VolUp = 0xAF
Media_VolDw = 0xAE




###SpotifyInfo###
def getwindow(Title="SpotifyMainWindow"):
	window_id = win32gui.GetWindowRect(win32gui.FindWindowEx(win32gui.FindWindow('TdxW_MainFrame_Class','???????V7.35 - [???-?????]'),None,'#32770',None))
	window_id = win32gui.FindWindow(Title, None)
	return window_id
	
def song_info():
	try:
		song_info = win32gui.GetWindowText(getwindow())
	except:
		pass
	return song_info

def artist():
	try:
		temp = song_info()
		artist, song = temp.split(" - ",1)
		artist = artist.strip()
		return artist
	except:
		return "There is noting playing at this moment"
	
def song():
	try:
		temp = song_info()
		artist, song = temp.split(" - ",1)
		song = song.strip()
		return song
	except:
		return "There is noting playing at this moment"
	
###SpotifyBlock###
def createfolder(folder_path="C:\SpotiBlock"):
	if not os.path.exists(folder_path):
		os.makedirs(folder_path)
	else:
		pass
	
def createfile(file_path="C:\SpotiBlock\Block.txt" ):
	if not os.path.exists(file_path):
		file = open(file_path, "a")
		file.write("ThisFirstLineWillBeIgnoredButIsNecessaryForUse")

def blocklist(file_path="C:\SpotiBlock\Block.txt"):
	block_list = []
	for line in open(file_path, "r"):
		if not line == "":
			block_list.append(line.strip())
	return block_list
	
def add_to_blocklist(file_path="C:\SpotiBlock\Block.txt"):
	with open(file_path, 'a') as text_file:
		text_file.write("\n" + song_info())
		
def reset_blocklist(file_path="C:\SpotiBlock\Block.txt"):
	with open(file_path, 'w') as text_file:
		text_file.write("ThisFirstLineWillBeIgnored")
		pass
	
	

	
###Media Controls###
def hwcode(Media):
	hwcode = win32api.MapVirtualKey(Media, 0)
	return hwcode

def next():
	win32api.keybd_event(Media_Next, hwcode(Media_Next))
	
def previous():
	win32api.keybd_event(Media_Previous, hwcode(Media_Previous))
	
def pause():
	win32api.keybd_event(Media_Pause, hwcode(Media_Pause))
	
def play():
	win32api.keybd_event(Media_Pause, hwcode(Media_Pause))
	
def mute():
	win32api.keybd_event(Media_Mute, hwcode(Media_Mute))

def volUp():
	win32api.keybd_event(Media_VolUp, hwcode(Media_VolUp))
	
def volDw():
	win32api.keybd_event(Media_VolDw, hwcode(Media_VolDw))


def SpotifyStart():
	spoti.start("Spotify.exe")

def oioi():
	#win32api.keybd_event(getwindow(), )
	win32api.keybd_event(clr, hwcode(clr))
	win32api.keybd_event(Ramdom_mus, hwcode(Ramdom_mus))

def vol100():
			volume.SetMasterVolumeLevel(0, None)

def vol90():

			volume.SetMasterVolumeLevel(-1.6, None)

def vol80():

			volume.SetMasterVolumeLevel(-3.4, None)

def vol70():

			volume.SetMasterVolumeLevel(-5.4, None)

def vol60():

			volume.SetMasterVolumeLevel(-7.8, None)

def vol50():

			volume.SetMasterVolumeLevel(-10.4, None)

def vol40():

			volume.SetMasterVolumeLevel(-13.8, None)

def vol30():

			volume.SetMasterVolumeLevel(-17.9, None)

def vol20():

			volume.SetMasterVolumeLevel(-23.9, None)

def vol10():

			volume.SetMasterVolumeLevel(-33.9, None)

