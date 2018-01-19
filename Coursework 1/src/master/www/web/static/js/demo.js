$(document).ready ( function () {
    $(document).on ("click", "#1,#2,#3,#4,#5,#6,#7,#8,#9,#10,#11,#12,#13,#14,#15,#16,#17,#18,#19", function () {
        var src = ($(this).attr('src') === "/static/img/no.png" )
           ? "/static/img/yes.png"
           : "/static/img/no.png";
        $(this).attr('src', src);
    });
});
