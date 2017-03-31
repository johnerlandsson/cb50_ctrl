//The module
angular.module("RecipeApp", ['ui-notification']).controller("RecipeCtrl", function($scope, $http, Notification) {
	$scope.pd = {};

  function get_pd() {
    $http.get("get_pd").then(function successCallback(response) {
			$scope.pd = response.data;
    }, function errorCallback(response) {
      Notification.error({
        message: "Failed to receive process data from server."
      });
    });
  }
	function update_pd() {
		get_pd();
	}

	window.setInterval(update_pd, 500);

  $scope.pump_btn_clicked = function() {
		console.log($scope.pd["pump"]);
  }
  $scope.run_btn_clicked = function() {
		console.log($scope.pd["run_recipe"]);
  }
  $scope.mixer_btn_clicked = function() {
		console.log($scope.pd["mixer"]);
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
