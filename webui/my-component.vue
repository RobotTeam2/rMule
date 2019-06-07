<template>
  <div class="card">
    <div class="card-body">
      <h5 class="card-title">Channel {{ index }}</h5>
      <div class="row justify-content-center">
        <div class="col">
          <div class="input-group mb-3">
            <div class="input-group-prepend">
              <span class="input-group-text">@</span>
            </div>
              <input type="text" class="form-control" placeholder="leg id">
          </div>
        </div>
        <div class="col">
          <div class="input-group mb-3">
            <div class="input-group-prepend">
              <span class="input-group-text">@</span>
            </div>
              <input type="text" class="form-control" placeholder="Wheel Max Front">
          </div>
        </div>
        <div class="col">
          <div class="input-group mb-3">
            <div class="input-group-prepend">
              <span class="input-group-text">@</span>
            </div>
              <input type="text" class="form-control" placeholder="Wheel Max Back">
          </div>
        </div>
      </div>
    </div>
  </div>
</template>
