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

import { TetrisViewModel, GameObject } from '../Model/TetrisViewModel'
import { NextFigureBarView } from './NextFigureBarView'

@Component
export struct StatusBarView {
  @StorageLink('game') game: TetrisViewModel = new TetrisViewModel(new GameObject)

  build() {
    Column() {
      Text('Level')
        .fontSize('20fp')
      Blank().height('4%')
      Text(this.game.gameObject.level.toString())
        .fontSize('20fp')
      Blank().height('14%')

      Text('Score')
        .fontSize('20fp')
      Blank().height('4%')
      Text(this.game.gameObject.score.toString())
        .fontSize('20fp')
      Blank().height('14%')
    }
    .width('100%')
    .height('35%')
  }
}