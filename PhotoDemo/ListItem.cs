using System;
using System.Collections.Generic;
using System.Text;

namespace PhotoDemo
{
    public class ListItem
    {
        private string name;
        private string value;
        public ListItem(string name, string value)
        {
            this.name = name;
            this.value = value;
        }

        public override string ToString()
        {
            return this.name;
        }

        public string Name
        {
            get
            {
                return this.name;
            }
            set
            {
                this.name = value;
            }
        }
        public string Value
        {
            get
            {
                return this.value;
            }
            set
            {
                this.value = value;
            }
        }
    }
}
