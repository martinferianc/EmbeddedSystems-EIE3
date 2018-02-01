var serverTime = new Date();

function updateTime() {
    /// Increment serverTime by 1 second and update the html for '#time'
    serverTime = new Date(serverTime.getTime() + 1000);
    $('#time').html(serverTime.toGMTString());
}

$(function() {
    updateTime();
    setInterval(updateTime, 1000);
})

var previous = null;
var current = null;
    setInterval(function() {
        $.get("data.html", function(data) {
            current = data;
            if ((previous && current) && (previous !== current)) {
                console.log('refresh');
                location.reload();
            }
            previous = current;
        });
    }, 1000);
