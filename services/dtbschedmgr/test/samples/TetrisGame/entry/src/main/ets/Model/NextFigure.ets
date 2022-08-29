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

import { GameObject } from './TetrisViewModel'
import { FigureTemplates } from './FigureTemplates'

export class NextFigureObject {
  figureType: FigureTemplates = new FigureTemplates
  typeNum: number = 2
  width: number = 4
  height: number = 4
  container: string[][] = []
}

export class NextFigure {
  gameObject: GameObject

  constructor(gameObject: GameObject, template: number) {
    this.gameObject = gameObject
    this.gameObject.modelObject.nextFigureObject.typeNum = template
    this.containerInit(this.gameObject.modelObject.nextFigureObject.figureType.figureType[this.gameObject.modelObject.nextFigureObject.typeNum - 1])
  }

  containerInit(template: string) {
    this.gameObject.modelObject.nextFigureObject.container = []
    var j = 0
    var temp: string[] = []
    for (var c of template) {
      temp.push(c)
      j += 1
      if (j == this.gameObject.modelObject.nextFigureObject.width) {
        this.gameObject.modelObject.nextFigureObject.container.push(temp)
        temp = []
        j = 0
      }
    }
  }
}