# [Click your heels to get home with these Uber ruby slippers](https://www.newscientist.com/article/mg23431273-000-do-try-this-at-home/)

By cobbling together some souped-up shoes, you can do a Dorothy and whizz yourself home at the end of a night on the town
##### A project by Hannah Joshua/New Scientist

## What is it?


This is the code I put together for a pair of shoes that call you an Uber home when you click your heels together three times within 1.5 seconds. You can find the original New Scientist article about the project [here](https://www.newscientist.com/article/mg23431273-000-do-try-this-at-home/).

[And also the video](https://www.facebook.com/newscientist/videos/10155699998299589/).

This project is made with Arduino Uno. I also added a 1Sheeld – which sits on top of the Arduino and allows you to use functions from your smartphone in your sketch over bluetooth. And of course, I also used Uber’s API.

## Parts list


- Shoes. (obviously)
- Arduino
- 1Sheeld
- large latching button
- battery (I used a standard 9v in a battery case with an on-off switch)
- LEDs
- wires
- smartphone with GPS and 1Sheeld app

## Basics


I aimed for a pair of shoes with a bit of a platform and a heel – this gave me room to put the Arduino in the hollowed out toe section of one shoe and hide the battery box in the heel. The latching switch is also set into the heel, and the LEDs are attached to the front of the shoe so they’re easily visible when you look down.

All the components are in one shoe – I thought it was best to keep the activation simple and contain all the electronics in one place. Splitting components between shoes adds complexity and more opportunity for things to go wrong.

Essentially, the code makes http requests to Uber’s API endpoints to order a cab to a hardcoded home destination. The 1Sheeld connects to your phone over bluetooth to pick up your GPS location and uses your mobile data signal to send the http request. 

[There’s lots of documentation on the 1Sheeld website about the libraries it comes with](https://1sheeld.com/)

[And there is lots of information on Uber’s API on the developer site](https://developer.uber.com/)

The code isn’t particularly elegant, but it does the job and this was one of my first Arduino projects, so please excuse the clunkiness! 

## Some notes


At the moment, this is kind of a basic version of the project; I was constrained by the amount of memory on the Arduino Uno and also by time. 

This means the shoes only take you home – i.e. the destination coordinates are hard coded. You have to plug the shoes into your computer and tweak the code before your night out if you want to change the destination. I reasoned that this was fine, seeing as adding some kind of input to the shoes spoils the idea of simply clicking your heels to go home and would just be like using the Uber app but worse.

Also, the shoes only work in a particular region. There’s a piece of information that Uber needs in the ride request, and it’s a code that applies to the region of service, presumably to tell Uber which fleet of cabs it needs to send the request to. It’s possible to get this info for your current location, but it requires another http request (which means time and memory) and it’s reasonably safe to assume that you’re not going so far from the hardcoded home destination that you’ll need a different region code. So find out your region code on a computer beforehand and drop it into the code/update the shoes.

I’ve received some comments about how I could/should have used a reed switch as the trigger instead of the latching button. It did cross my mind, but I figured that calibration would be trickier – it would be hard to guard against accidental activation when you stood with your feet close together. Also, the tactile feedback you get from a chunky button should reduce the chance of accidental activation by letting you know you’ve “clicked your heels”. You don’t want to lower your star rating by accidentally calling cabs all the time!

You can find some info about Oauth2 [here](https://aaronparecki.com/oauth-2-simplified/) – they will explain how to get your access token better than I can here. 