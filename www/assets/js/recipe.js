//The module
angular.module("RecipeApp", ['ui-notification']).controller("RecipeCtrl", function($scope, $http, Notification) {
  $scope.pump = false;

  $scope.pump_btn_clicked = function() {
    console.log("pump");
  }
  $scope.run_btn_clicked = function() {
    console.log("run");
  }
  $scope.mixer_btn_clicked = function() {
    console.log("mixer");
  }
}).config(function(NotificationProvider) { //Configure notifications
  NotificationProvider.setOptions({
    delay: 10000,
    startTop: 20,
    startRight: 10,
    verticalSpacing: 20,
    horizontalSpacing: 20,
    positionX: 'left',
    positionY: 'bottom'
  });
});
