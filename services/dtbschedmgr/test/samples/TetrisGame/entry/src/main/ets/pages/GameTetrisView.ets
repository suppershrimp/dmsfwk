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

import router from '@ohos.router';
import { GameObject } from '../Model/TetrisViewModel'
import { NextFigureBarView } from './NextFigureBarView'
import { TetrisViewModel, State } from '../Model/TetrisViewModel'
import { MenuView } from './MenuView'
import { GameView } from './GameView'
import { StatusBarView } from './StatusBarView'
import { ControlPanelView } from './ControlPanelView'

@Entry
@Component
struct Index {
  @StorageLink('state') state: State = new State()
  @StorageLink('game') game: TetrisViewModel = new TetrisViewModel(new GameObject)

  build() {
    Row() {
      Column() {
        Blank().height('3%')
        Row() {
          Button() {
            Text(' < Back')
              .fontSize('20fp')
              .fontWeight(FontWeight.Bold)
              .fontColor(Color.Blue)
          }
          .backgroundColor('')
          .onClick(() => {
            router.back()
          })

          Blank().width('95%')
        }

        Blank().height('9%')
        Text('TETRIS')
          .textAlign(TextAlign.Center)
          .fontWeight(FontWeight.Bold)
          .fontColor(Color.Gray)
          .fontSize('30fp')
        Blank().height('4%')
        Stack() {
          Rect().fill(Color.Gray).width('92%').height('55%')
          Row() {
            Stack() {
              GameView()
              if (this.state.isMenu) {
                MenuView()
              }
            }
            .width('56%')
            .height('52%')

            Rect()
              .fill(Color.Black)
              .width('1%')
              .height('52%')

            Column() {
              StatusBarView()
              Blank().height('2.5%')
              NextFigureBarView()
              Blank().height('20%')

              Button('ButtonType.Circle') {
                Stack() {
                  Circle().width('80%').height('80%').fill('rgb(146,162,154)').antiAlias(true)
                  if (this.state.isPause) {
                    Polygon().points([[3, 2], [18, 11], [3, 20]])
                  }
                  else {
                    Row() {
                      Rect().width(5).height(20)
                      Blank().width('20%')
                      Rect().width(5).height(20)
                    }
                  }
                }
              }
              .backgroundColor(Color.Black)
              .width('40%')
              .height('10%')
              .enabled(this.state.isStarted)
              .onClick((event: ClickEvent) => {
                if (!this.game.pause()) {
                  this.game.pauseBack()
                }
              })

              Blank().height('5%')
            }
            .width('28%')
            .height('52%')
            .backgroundColor('rgb(159,182,171)' +
            '')
          }
        }

        Blank().height('4%')
        ControlPanelView()
      }
      .width('100%')
      .height('100%')
    }
    .width('100%')
    .height('100%')
    .backgroundColor('rgb(219,219,219)')
    .onDisAppear(() => {
      if (this.state.isStarted) {
        this.game.updateHistory()
      }
      this.game.pause()
    })
  }
}