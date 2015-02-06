var myname=0;
var chatfriend=0;
var chatfriendname='';

var host='http://192.168.122.232:3000/'

function nowSeconds() {
	var date = new Date();
	return parseInt(date.getTime()/1000);
}


function getFriendList() {
	
	$.get(host + 'friendlist?accid=1&zoneid=10', function(data, status) {
		if (data.friends) {
			for (i in data.friends) {
				$('#friendlist').append('<a href="#" id="' + data.friends[i].charid + '" class="list-group-item">' + data.friends[i].name + '</a>')
			}
		}

		registerFriendListEvent();
	});

/*	
	for (i = 0; i < 20; ++ i) {
		$('#friendlist').append('<a href="#" id="' + i + '" class="list-group-item">David' + i + '</a>');
	}

	registerFriendListEvent();
*/
}

function sendMsg() {
	if (!myname) {
		alert('Please Input YourName First !!!');
		return
	}

	var chat = $('#chatcontent').val();
	$('#chatcontent').val('');

	console.log(chat);

	$.get(host + 'friendchat?charid=' + myname 
		+ '&friend=' + chatfriend
		+ '&friendname=' +  chatfriendname
		+ '&chat=' + escape(chat));

	showMyMsg(chat, 'Me', nowSeconds());
}

function pullMsg() {
	if (!myname) return;

	$.get(host + 'pullchat?charid=' + myname, function(data, status){
		//console.log(data);
		if (data.chats) {
			for (i in data.chats) {
				showFriendMsg(data.chats[i].content,
					data.chats[i].name,
					data.chats[i].timestamp);
			}
		}
	})
}

function timeTick() {
	pullMsg();
}

function registerFriendListEvent() {
	$('.list-group-item').click(function() {
		$(".list-group-item").removeClass('active');
		$(this).addClass('active');

		chatfriend = $(this).attr('id');
		chatfriendname = $(this).text();

		$("#chatfriend").text(chatfriendname);

		console.log(chatfriendname);
	});
}

function showMyMsg(msg, name, timestamp) {
	var d = new Date();
	d.setTime(timestamp*1000);

	$('#content').append('<div class="alert alert-success mymsg" role="alert">' 
		+ name + "(" + d.toTimeString() + "):<br> " + msg + '</div>');	

	scrollDown();
}

function showFriendMsg(msg, name, timestamp) {

	var d = new Date();
	d.setTime(timestamp*1000);

	$('#content').append('<div class="alert alert-info friendmsg" role="alert">' 
		+ name + "(" + d.toTimeString() + "):<br> " + msg + '</div>');	

	scrollDown();
}

function scrollDown() {
	 $('#content').scrollTop( $('#content')[0].scrollHeight );
}

$(document).ready(function(){
	$(".chatmain").height($(window).height() - 180);


/*
	for (i = 0; i < 100; ++ i) {
		if (i % 2 == 0)
			showMyMsg('only for test ...')
		else
			showFriendMsg('hello ...');
	}
	*/


	$('#myname').bind('keypress',function(event){
        if(event.keyCode == "13")  {
        	myname = $('#myname').val();
        }
    });

    $('#chatcontent').bind('keypress',function(event){
        if(event.keyCode == "13")  {
        	sendMsg();
        }
    });

    $('#btnsend').click(function(){
    	sendMsg();
    });

    $('#myname').focus();

    setInterval(timeTick, 1000); 

    getFriendList();

    $('#content').scrollTop( $('#content')[0].scrollHeight );
});

window.onresize = function() {
	$(".chatmain").height($(window).height() - 180);
	$('#content').scrollTop( $('#content')[0].scrollHeight );
}