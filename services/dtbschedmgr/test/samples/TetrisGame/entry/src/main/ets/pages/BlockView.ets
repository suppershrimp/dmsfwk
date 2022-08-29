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

import { BlockState } from '../Model/BlockState';

@Component
export struct BlockView {
  @State x: number = 0
  @State y: number = 0
  @State length: number = 20
  @State opacity: number = 0

  build() {
    Stack() {
      if (this.opacity == 1) {
        Rect()
          .width('95%')
          .height('95%')
          .strokeWidth('10%')
          .fill('')
          .stroke(Color.Black)
        Rect()
          .width('50%')
          .height('50%')
          .fill(Color.Black)
      }
      else if (this.opacity == 0.3) {
        Rect()
          .width('95%')
          .height('95%')
          .strokeWidth('10%')
          .fill('')
          .stroke('rgb(111,128,119)')
        Rect()
          .width('50%')
          .height('50%')
          .fill('rgb(111,128,119)')
      }
      else if (this.opacity == 0.1) {
        Rect()
          .width('95%')
          .height('95%')
          .strokeWidth('10%')
          .fill('')
          .stroke('rgb(144,164,153)')
        Rect()
          .width('50%')
          .height('50%')
          .fill('rgb(144,164,153)')
      }
    }
    .width('100%')
    .height('100%')
  }
}