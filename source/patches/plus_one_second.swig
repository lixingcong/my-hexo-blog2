<script>

var click_counter=0;
$("body").bind("click",function(e){
	var $i=$("<elder>").text("+"+(++click_counter)+"s");
	var x=e.pageX, y=e.pageY;
	
	$i.css({
		"z-index":99999,
		"top":y-15,
		"left":x,
		"position":"absolute",
		"color":"red"
	});
	
	$("body").append($i);
	
	$i.animate(
		{"top":y-180,"opacity":0},
		1500,
		function(){$i.remove();}
	);
	
	e.stopPropagation();
});

</script>
