function replace_level(log = []) {
  for (i = 0; i < log.length; i++) {
    switch (log[i]['level']) {
      case 1:
        log[i]['level'] = 'fa-minus-circle';
        break;
      case 2:
        log[i]['level'] = 'fa-exclamation-triangle';
        break;
      case 3:
        log[i]['level'] = 'fa-bug';
        break;
      default:
        log[i]['level'] = 'fa-info-circle';
        break;
    }
  }
}

angular.module('LogApp', ['ui-notification'])
  .controller(
    'LogCtrl',
    function($scope, $http, Notification) {
      $scope.rdo_all = 1;
      // Function for fetching log data
      function fetch_log(level = -1) {
        var path = 'get_log';
        if (level >= 0 && level <= 3) {
          path = path.concat('/');
          path = path.concat(level.toString());
        }
        $http.get(path).then(
          function successCallback(response) {
            $scope.log = response.data['data'];
            replace_level($scope.log);
          },
          function errorCallback(response) {
            Notification.error({
              message: 'Failed to receive log.'
            });
          });
      };

      //Fetch all log levels on load
      window.onload = fetch_log;

      $scope.all_clicked = function() {
        fetch_log();
      };
      $scope.info_clicked = function() {
        fetch_log(0);
      };
      $scope.error_clicked = function() {
        fetch_log(1);
      };
      $scope.warning_clicked = function() {
        fetch_log(2);
      };
      $scope.debug_clicked = function() {
        fetch_log(3);
      };
    })
  .config(function(NotificationProvider) { // Configure notifications
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
