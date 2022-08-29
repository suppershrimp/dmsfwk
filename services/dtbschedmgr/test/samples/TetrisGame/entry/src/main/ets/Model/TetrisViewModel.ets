/**
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { GameModel, ModelObject } from './GameModel'
import { GameState } from './GameState'

export class GameObject {
  rows: number = 20
  columns: number = 10
  speed: number = 0.9
  score: number = 0
  level: number = 1
  timeOutId: number = 0
  modelObject: ModelObject = new ModelObject
  history: number[] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
  index: number = 10
}

export class State {
  isMenu: boolean = true
  isStarted: boolean = false
  isPause: boolean = false
  isEnd: boolean = false
  update: boolean = true
  isInEvent: boolean = false
}

export class TetrisViewModel {
  gameObject: GameObject
  model: GameModel

  constructor(gameObject: GameObject) {
    this.gameObject = gameObject
    this.model = new GameModel(this.gameObject)
    this.gameObject.modelObject = this.model.gameObject.modelObject
  }

  newGame() {
    this.model = new GameModel(new GameObject)
    this.gameObject.modelObject = this.model.gameObject.modelObject
    this.gameObject.speed = 0.9
    this.gameObject.level = 1
    this.gameObject.score = 0
    var state: State = AppStorage.Get<State>('state')
    state.isMenu = false
    state.isStarted = true
    state.isPause = false
    state.isEnd = false
    state.update = true
    state.isInEvent = false
    AppStorage.Set<State>('state', state)
    this.play()
  }

  play() {
    let that = this
    this.gameObject.timeOutId = setInterval(function () {
      that.move()
    }, this.gameObject.speed * 1000)
  }

  move() {
    while (AppStorage.Get<State>('state').isInEvent) {
    }
    var state1 = AppStorage.Get<State>('state')
    state1.isInEvent = true
    AppStorage.Set<State>('state', state1)

    var returnState: GameState = this.model.move()

    if (returnState == GameState.lineBurning) {
      this.burningLineEvent()
    }
    else if (returnState == GameState.endGame) {
      this.onEndGame()
    }
    this.updateScreen()

    var state2: State = AppStorage.Get<State>('state')
    state2.isInEvent = false
    AppStorage.Set<State>('state', state2)
  }

  burningLineEvent() {
    this.gameObject.score += 50
    if (this.gameObject.score >= 500 * this.gameObject.level) {
      this.gameObject.level += 1
      this.gameObject.speed *= 0.9
      clearInterval(this.gameObject.timeOutId)
      this.play()
    }
  }

  onEndGame() {
    clearInterval(this.gameObject.timeOutId)
    this.updateHistory()
    this.gameObject.index = 10
    var state: State = AppStorage.Get<State>('state')
    state.isMenu = true
    state.isStarted = false
    state.isPause = false
    state.isEnd = true
    state.update = true
    state.isInEvent = false
    AppStorage.Set<State>('state', state)
  }

  updateHistory() {
    let temp = this.gameObject.score
    if (this.gameObject.history[9] > temp) {
      return
    }
    var i = Math.min(this.gameObject.index - 1, 8)
    for (; i >= 0; i--) {
      if (this.gameObject.history[i] <= temp) {
        this.gameObject.history[i+1] = this.gameObject.history[i]
      } else {
        break
      }
    }
    this.gameObject.index = i + 1
    this.gameObject.history[i+1] = temp
  }

  updateScreen() {
    var state1: State = AppStorage.Get<State>('state')
    state1.update = false
    AppStorage.Set<State>('state', state1)
    this.model.update()
    var state2: State = AppStorage.Get<State>('state')
    state2.update = true
    AppStorage.Set<State>('state', state2)
  }

  pause(): boolean {
    var state: State = AppStorage.Get<State>('state')
    if (state.isStarted && !state.isPause) {
      state.isPause = true
      state.isMenu = true
      state.isInEvent = false
      AppStorage.Set<State>('state', state)
      clearInterval(this.gameObject.timeOutId)
      return true
    }
    return false
  }

  pauseBack() {
    var state: State = AppStorage.Get<State>('state')
    if (state.isStarted && !state.isPause) {
      state.isPause = false
      state.isMenu = false
      AppStorage.Set<State>('state', state)
      this.play()
    }
  }
}