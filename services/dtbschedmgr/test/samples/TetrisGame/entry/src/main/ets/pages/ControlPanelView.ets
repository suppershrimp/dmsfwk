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

import { GameObject } from '../Model/TetrisViewModel';
import { TetrisViewModel, State } from '../Model/TetrisViewModel';

@Component
export struct ControlPanelView {
  @StorageLink('state') state: State = new State
  @StorageLink('game') game: TetrisViewModel = new TetrisViewModel(new GameObject)
  isInAButton: boolean = false

  build() {
    Column() {
      Row() {
        Button('ButtonType.Circle') {
          Stack() {
            Circle()
              .fill('rgb(255,204,0)')
              .width('85%')
              .height('85%')
            Polygon().points([[30, 0], [0, 15], [30, 30]]).fill('rgb(243, 138, 0)')
          }
        }
        .backgroundColor('rgb(172,172,171)')
        .width('21%')
        .height('35%')
        .onClick((event: ClickEvent) => {
          if (this.state.isPause || this.state.isMenu || this.state.isInEvent || this.state.isEnd || this.isInAButton) {
            return
          }
          this.isInAButton = true
          this.state.isInEvent = true
          this.game.model.figure.moveLeft()
          this.game.updateScreen()
          this.state.isInEvent = false
          this.isInAButton = false
        })

        Blank().width(10)
        Button('ButtonType.Circle') {
          Stack() {
            Circle()
              .fill('rgb(255,204,0)')
              .width('85%')
              .height('85%')
            Polygon().points([[10, 0], [40, 15], [10, 30]]).fill('rgb(243, 138, 0)')
          }
        }
        .backgroundColor('rgb(172,172,171)')
        .width('21%')
        .height('35%')
        .onClick((event: ClickEvent) => {
          if (this.state.isPause || this.state.isMenu || this.state.isInEvent || this.state.isEnd || this.isInAButton) {
            return
          }
          this.isInAButton = true
          this.state.isInEvent = true
          this.game.model.figure.moveRight()
          this.game.updateScreen()
          this.state.isInEvent = false
          this.isInAButton = false
        })

        Blank().width(80)
        Button('ButtonType.Circle') {
          Stack() {
            Circle()
              .fill('rgb(47,163,225)')
              .width('85%')
              .height('85%')
            Line()
              .width(30)
              .height(30)
              .startPoint([15, 0])
              .endPoint([15, 30])
              .strokeWidth(3)
            Line()
              .width(30)
              .height(30)
              .startPoint([0, 30])
              .endPoint([30, 30])
              .strokeWidth(3)
            Line()
              .width(30)
              .height(30)
              .startPoint([0, 20])
              .endPoint([15, 30])
              .strokeWidth(3)
            Line()
              .width(30)
              .height(30)
              .startPoint([30, 20])
              .endPoint([15, 30])
              .strokeWidth(3)
          }
        }
        .backgroundColor('rgb(172,172,171)')
        .width('21%')
        .height('35%')
        .onClick((event: ClickEvent) => {
          if (this.state.isPause || this.state.isMenu || this.state.isInEvent || this.state.isEnd || this.isInAButton) {
            return
          }
          this.isInAButton = true
          this.state.isInEvent = true
          this.game.model.figure.fastMoveDown()
          this.game.updateScreen()
          this.state.isInEvent = false
          this.game.move()
          this.isInAButton = false
        })
      }

      Row() {
        Button('ButtonType.Circle') {
          Stack() {
            Circle()
              .fill('rgb(255,204,0)')
              .width('85%')
              .height('85%')
            Polygon().points([[0, 0], [30, 0], [15, 30]]).fill('rgb(243, 138, 0)')
          }
        }
        .backgroundColor('rgb(172,172,171)')
        .width('21%')
        .height('35%')
        .onClick((event: ClickEvent) => {
          if (this.state.isPause || this.state.isMenu || this.state.isInEvent || this.state.isEnd || this.isInAButton) {
            return
          }
          this.isInAButton = true
          this.state.isInEvent = true
          this.game.model.figure.moveDown()
          this.game.updateScreen()
          this.state.isInEvent = false
          this.isInAButton = false
        })

        Blank().width(75)
        Button('ButtonType.Circle') {
          Stack() {
            Circle()
              .fill('rgb(47,163,225)')
              .width('85%')
              .height('85%')
            Circle()
              .fill('')
              .width(30)
              .height(30)
              .stroke(Color.Black)
              .strokeWidth(3)
              .antiAlias(true)
            Line()
              .width(40)
              .height(40)
              .startPoint([15, -1])
              .endPoint([10, 10])
              .strokeWidth(3)
            Line()
              .width(40)
              .height(40)
              .startPoint([20, 15])
              .endPoint([10, 10])
              .strokeWidth(3)
            Polygon().width(40).height(40).points([[5, 10], [21, 18], [0, 30]]).fill('rgb(47, 163, 225)')
          }
        }
        .backgroundColor('rgb(172,172,171)')
        .width('21%')
        .height('35%')
        .onClick((event: ClickEvent) => {
          if (this.state.isPause || this.state.isMenu || this.state.isInEvent || this.state.isEnd || this.isInAButton) {
            return
          }
          this.isInAButton = true
          this.state.isInEvent = true
          this.game.model.figure.rotate()
          this.game.updateScreen()
          this.state.isInEvent = false
          this.isInAButton = false
        })
      }
    }
    .width('92%')
    .height('25%')
  }
}