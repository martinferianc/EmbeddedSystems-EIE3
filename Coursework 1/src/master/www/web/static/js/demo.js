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
